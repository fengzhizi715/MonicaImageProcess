#!/bin/bash
set -e

# 📦 安装路径和构建路径
INSTALL_DIR="${HOME}/Libs/heif-suite"
BUILD_DIR="${HOME}/Libs/heif-build"
TOOLS_DIR="${HOME}/Libs/heif-tools"
NUM_JOBS=$(sysctl -n hw.ncpu)

# 版本定义
ZLIB_VERSION="1.3.1"
LIBPNG_VERSION="1.6.43"
AOM_VERSION="3.8.1"
DAV1D_VERSION="1.4.1"
LIBDE265_VERSION="1.0.12"
LIBHEIF_VERSION="1.19.8"
NASM_VERSION="2.16.01"
MESON_VERSION="1.4.0"
NINJA_VERSION="1.11.1"

# 初始化目录
mkdir -p "${INSTALL_DIR}" "${BUILD_DIR}" "${TOOLS_DIR}"

# ===== 安装独立的构建工具 =====
echo "🛠️ 安装独立构建工具..."
export PATH="${TOOLS_DIR}/bin:${PATH}"

# 确保有 Python
if ! command -v python3 &> /dev/null; then
    echo "⚠️ 未找到 python3，尝试安装..."
    brew install python || {
        echo "❌ 无法安装 Python"
        exit 1
    }
fi

# 安装 meson 和 ninja 到独立目录
if ! command -v meson &> /dev/null || ! meson --version | grep -q ${MESON_VERSION}; then
    echo "📦 安装 meson ${MESON_VERSION}..."
    python3 -m pip install --prefix="${TOOLS_DIR}" "meson==${MESON_VERSION}"
fi

if ! command -v ninja &> /dev/null || ! ninja --version | grep -q ${NINJA_VERSION}; then
    echo "📦 安装 ninja ${NINJA_VERSION}..."
    python3 -m pip install --prefix="${TOOLS_DIR}" "ninja==${NINJA_VERSION}"
fi

# 修复：设置 PYTHONPATH 以便 Meson 能找到其模块
PYTHON_VERSION=$(python3 -c "import sys; print(f'python{sys.version_info.major}.{sys.version_info.minor}')")
PYTHON_SITE_PACKAGES="${TOOLS_DIR}/lib/${PYTHON_VERSION}/site-packages"
export PYTHONPATH="${PYTHON_SITE_PACKAGES}:${PYTHONPATH}"
echo "设置 PYTHONPATH: ${PYTHONPATH}"

# 验证工具安装
if ! meson --version; then
    echo "❌ 错误：meson 安装失败"
    exit 1
fi

if ! ninja --version; then
    echo "❌ 错误：ninja 安装失败"
    exit 1
fi

echo "✅ 构建工具准备就绪: meson $(meson --version), ninja $(ninja --version)"

# 检测架构并设置编译标志
ARCH=$(uname -m)
if [ "$ARCH" = "arm64" ]; then
    echo "🍏 检测到 Apple Silicon (ARM64) 架构"
    # 禁用有问题的 ARM 汇编优化
    LIBDE265_EXTRA_FLAGS="--disable-arm"
    # 为 ARM64 设置特定优化标志
    OPT_FLAGS="-O3 -fPIC -mcpu=apple-m1 -mtune=apple-m1"
else
    echo "💻 检测到 Intel 架构"
    LIBDE265_EXTRA_FLAGS=""
    OPT_FLAGS="-O3 -fPIC"
fi

# 构建环境变量
export MACOSX_DEPLOYMENT_TARGET="10.15"
export CFLAGS="${OPT_FLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
export CXXFLAGS="${CFLAGS}"
export LDFLAGS="-mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
export PKG_CONFIG_PATH="${INSTALL_DIR}/lib/pkgconfig:${PKG_CONFIG_PATH}"
export PATH="${INSTALL_DIR}/bin:${TOOLS_DIR}/bin:${PATH}"

# 通用解压函数
extract_archive() {
  local file=$1
  case "$file" in
    *.tar.gz|*.tgz) tar -xzf "$file" -C "$BUILD_DIR" ;;
    *.tar.xz)       tar -xJf "$file" -C "$BUILD_DIR" ;;
    *.zip)          unzip "$file" -d "$BUILD_DIR" ;;
    *)              echo "❌ 不支持的压缩格式: $file" ; exit 1 ;;
  esac
}

# 改进的下载和解压函数
download_and_extract() {
  local name=$1 version=$2 url=$3
  local tar_file="${BUILD_DIR}/${name}-${version}.tar.gz"
  local src_dir="${BUILD_DIR}/${name}-${version}"

  # 检查源码目录是否已存在
  if [ -d "$src_dir" ]; then
    echo "✅ ${name}-${version} 源码已存在，跳过下载和解压"
    return 0
  fi

  # 检查压缩包是否已存在
  if [ -f "$tar_file" ]; then
    echo "✅ ${name}-${version} 压缩包已存在，跳过下载"
  else
    echo "📥 下载 ${name}-${version}..."
    curl -L -o "$tar_file" "$url" || {
      echo "❌ 下载失败: $url"
      rm -f "$tar_file"
      exit 1
    }
  fi

  echo "📦 解压 ${name}-${version}..."
  extract_archive "$tar_file"
}

# 检查库是否已安装
is_library_installed() {
  local name=$1 version=$2
  local lib_file="${INSTALL_DIR}/lib/lib${name}.a"

  # 特殊处理 zlib (库名为 libz.a)
  if [ "$name" = "zlib" ]; then
    lib_file="${INSTALL_DIR}/lib/libz.a"
  fi

  # 特殊处理 aom (库名为 libaom.a)
  if [ "$name" = "aom" ]; then
    lib_file="${INSTALL_DIR}/lib/libaom.a"
  fi

  if [ -f "$lib_file" ]; then
    echo "✅ ${name} ${version} 已安装，跳过编译"
    return 0
  fi

  return 1
}

cmake_build() {
  local dir=$1 options=$2
  mkdir -p "${dir}/build" && cd "${dir}/build"
  cmake .. -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DCMAKE_PREFIX_PATH="${INSTALL_DIR}" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ${options}
  make -j${NUM_JOBS}
  make install
}

autotools_build() {
  cd "$1"
  ./configure --prefix="${INSTALL_DIR}" $2
  make -j${NUM_JOBS}
  make install
}

# 清理旧构建缓存 (保留下载的压缩包)
if [ -d "${BUILD_DIR}" ]; then
  echo "🧹 清理旧构建目录 (保留压缩包)..."
  find "${BUILD_DIR}" -mindepth 1 -maxdepth 1 -type d -exec rm -rf {} +
fi

# 1. NASM
if ! is_library_installed "nasm" "$NASM_VERSION"; then
  download_and_extract "nasm" "$NASM_VERSION" "https://www.nasm.us/pub/nasm/releasebuilds/${NASM_VERSION}/nasm-${NASM_VERSION}.tar.gz"
  autotools_build "${BUILD_DIR}/nasm-${NASM_VERSION}" ""
fi

# 2. Zlib
if ! is_library_installed "zlib" "$ZLIB_VERSION"; then
  download_and_extract "zlib" "$ZLIB_VERSION" "https://zlib.net/zlib-${ZLIB_VERSION}.tar.gz"
  cd "${BUILD_DIR}/zlib-${ZLIB_VERSION}" && ./configure --prefix="${INSTALL_DIR}" --static && make -j${NUM_JOBS} && make install
fi

# 3. LibPNG
if ! is_library_installed "png" "$LIBPNG_VERSION"; then
  download_and_extract "libpng" "$LIBPNG_VERSION" "https://download.sourceforge.net/libpng/libpng-${LIBPNG_VERSION}.tar.gz"
  autotools_build "${BUILD_DIR}/libpng-${LIBPNG_VERSION}" "--disable-shared --enable-static --with-zlib-prefix=${INSTALL_DIR}"
fi

# 4. AOM
if ! is_library_installed "aom" "$AOM_VERSION"; then
  download_and_extract "aom" "$AOM_VERSION" "https://storage.googleapis.com/aom-releases/libaom-${AOM_VERSION}.tar.gz"

  # 添加 ARM64 特定优化
  AOM_EXTRA_FLAGS=""
  if [ "$ARCH" = "arm64" ]; then
    AOM_EXTRA_FLAGS="-DAOM_TARGET_CPU=arm64"
  fi

  cmake_build "${BUILD_DIR}/libaom-${AOM_VERSION}" "-DENABLE_TESTS=OFF -DENABLE_DOCS=OFF -DENABLE_TOOLS=OFF -DENABLE_EXAMPLES=OFF ${AOM_EXTRA_FLAGS}"
fi

# 5. DAV1D
if ! is_library_installed "dav1d" "$DAV1D_VERSION"; then
  download_and_extract "dav1d" "$DAV1D_VERSION" "https://code.videolan.org/videolan/dav1d/-/archive/${DAV1D_VERSION}/dav1d-${DAV1D_VERSION}.tar.gz"

  # 添加 ARM64 特定优化
  DAV1D_EXTRA_FLAGS=""
  if [ "$ARCH" = "arm64" ]; then
    DAV1D_EXTRA_FLAGS="-Denable_asm=false" # 暂时禁用汇编优化
  fi

  cd "${BUILD_DIR}/dav1d-${DAV1D_VERSION}" && meson setup build \
    --prefix="${INSTALL_DIR}" \
    --buildtype=release \
    --default-library=static \
    -Denable_tests=false \
    -Denable_tools=false \
    ${DAV1D_EXTRA_FLAGS}
  cd build && ninja -j${NUM_JOBS} && ninja install
fi

# 6. libde265 - 修复 ARM64 汇编问题
if ! is_library_installed "de265" "$LIBDE265_VERSION"; then
  download_and_extract "libde265" "$LIBDE265_VERSION" "https://github.com/strukturag/libde265/releases/download/v${LIBDE265_VERSION}/libde265-${LIBDE265_VERSION}.tar.gz"

  # 应用补丁修复 ARM64 汇编问题
  if [ "$ARCH" = "arm64" ]; then
    echo "🔧 修复 libde265 ARM64 汇编问题..."
    cd "${BUILD_DIR}/libde265-${LIBDE265_VERSION}"

    # 检查文件是否存在
    if [ -d "libde265" ]; then
        ARM_DIR="libde265/arm"
    elif [ -d "src" ]; then
        ARM_DIR="src/arm"
    else
        echo "⚠️ 警告: 无法确定 ARM 汇编文件位置，跳过修复"
        ARM_DIR=""
    fi

    # 替换有问题的汇编指令
    if [ -n "$ARM_DIR" ] && [ -f "${ARM_DIR}/arm-common.S" ]; then
        sed -i '' 's/bx lr/ret/g' "${ARM_DIR}/arm-common.S"
        sed -i '' 's/bx lr/ret/g' "${ARM_DIR}/arm-sao.S"
        sed -i '' 's/bx lr/ret/g' "${ARM_DIR}/arm-util.S"
        sed -i '' 's/bx lr/ret/g' "${ARM_DIR}/arm-sse-dsp.S"
        sed -i '' 's/bx lr/ret/g' "${ARM_DIR}/hevcdsp_qpel_neon.S"
        sed -i '' 's/\.endfunc//g' "${ARM_DIR}/hevcdsp_qpel_neon.S"
        echo "✅ ARM 汇编修复完成"
    else
        echo "⚠️ 警告: ARM 汇编文件未找到，跳过修复"
    fi
  fi

  autotools_build "${BUILD_DIR}/libde265-${LIBDE265_VERSION}" \
    "--disable-shared --enable-static --disable-sherlock265 ${LIBDE265_EXTRA_FLAGS}"
fi

# 7. libheif - 修复头文件安装问题
if ! is_library_installed "heif" "$LIBHEIF_VERSION"; then
  download_and_extract "libheif" "$LIBHEIF_VERSION" "https://github.com/strukturag/libheif/releases/download/v${LIBHEIF_VERSION}/libheif-${LIBHEIF_VERSION}.tar.gz"

  # 添加 ARM64 特定优化
  HEIF_EXTRA_FLAGS=""
  if [ "$ARCH" = "arm64" ]; then
    HEIF_EXTRA_FLAGS="-DWITH_DAV1D=OFF" # 如果 dav1d 有问题，暂时禁用
  fi

  # 修复：手动创建头文件目录
  mkdir -p "${INSTALL_DIR}/include/libheif"

  # 修改 CMake 命令确保头文件安装到正确位置
  cd "${BUILD_DIR}/libheif-${LIBHEIF_VERSION}"
  mkdir -p build && cd build
  cmake .. \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    -DCMAKE_PREFIX_PATH="${INSTALL_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DWITH_DAV1D=ON \
    -DWITH_AOM=ON \
    -DWITH_LIBDE265=ON \
    -DWITH_X265=OFF \
    -DWITH_EXAMPLES=OFF \
    -DWITH_LIBSHARPYUV=OFF \
    ${HEIF_EXTRA_FLAGS}

  # 显式设置头文件安装位置
  cmake --build . --target install --parallel ${NUM_JOBS}

  # 验证头文件是否安装
  if [ -f "${INSTALL_DIR}/include/libheif/heif.h" ]; then
      echo "✅ heif.h 已安装: ${INSTALL_DIR}/include/libheif/heif.h"
  else
      echo "❌ 错误: heif.h 未安装到正确位置"
      echo "尝试手动复制头文件..."

      # 从源目录复制头文件
      if [ -d "${BUILD_DIR}/libheif-${LIBHEIF_VERSION}/libheif" ]; then
          cp -r "${BUILD_DIR}/libheif-${LIBHEIF_VERSION}/libheif" "${INSTALL_DIR}/include/"
          echo "✅ 手动复制头文件到 ${INSTALL_DIR}/include/libheif"
      elif [ -d "${BUILD_DIR}/libheif-${LIBHEIF_VERSION}/src" ]; then
          # 尝试从 src 目录复制
          cp -r "${BUILD_DIR}/libheif-${LIBHEIF_VERSION}/src" "${INSTALL_DIR}/include/libheif"
          echo "✅ 手动复制头文件到 ${INSTALL_DIR}/include/libheif"
      else
          echo "❌ 无法找到 libheif 头文件源目录"
          exit 1
      fi
  fi
fi

# 验证
echo "🔍 验证构建结果:"
echo "头文件位置:"
find "${INSTALL_DIR}/include" -name "heif.h"

echo "库文件:"
find "${INSTALL_DIR}/lib" -name '*.a' | while read f; do
  echo -n "$f: "
  file "$f"
done

echo "✅ 所有库已构建成功并安装至: ${INSTALL_DIR}"
echo "   构建工具安装至: ${TOOLS_DIR}"
echo "   下次运行将跳过已安装的库"