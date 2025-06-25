#!/bin/bash
set -e

# OpenCV 版本
OPENCV_VERSION=4.10.0

# 自定义路径
OPENCV_SRC_DIR=~/Libs/opencv-src/opencv
OPENCV_CONTRIB_DIR=~/Libs/opencv-src/opencv_contrib
OPENCV_BUILD_DIR=~/Libs/opencv-build
INSTALL_DIR=~/Libs/opencv-install

# 创建目录
mkdir -p "$(dirname "$OPENCV_SRC_DIR")"
mkdir -p "$OPENCV_BUILD_DIR"
mkdir -p "$INSTALL_DIR"

# 检测架构
ARCH=$(uname -m)
if [[ "$ARCH" == "arm64" ]]; then
  BREW_PREFIX="/opt/homebrew"
else
  BREW_PREFIX="/usr/local"
fi
DEPLOY_TARGET="10.15"  # 设置兼容的最低 macOS 版本

# ===== 关键修复：完全禁用或内部构建 OpenEXR =====
echo "🔧 配置 OpenEXR 处理..."
# 选项1: 完全禁用 OpenEXR（推荐，除非您需要 EXR 格式支持）
EXR_OPTS=(
    -DWITH_OPENEXR=OFF
)

# 选项2: 强制使用内部构建（如果需要 EXR 支持）
# EXR_OPTS=(
#     -DWITH_OPENEXR=ON
#     -DBUILD_OPENEXR=ON
#     -DOPENEXR_ROOT="${OPENCV_SRC_DIR}/3rdparty/openexr"
#     -DOPENEXR_VERSION="2.3.0"  # OpenCV 4.10 使用的版本
# )

# ===== 关键修复：使用 OpenCV 内置的 Protobuf =====
echo "🔧 配置使用 OpenCV 内置 Protobuf..."
PROTOBUF_OPTS=(
  -DBUILD_PROTOBUF=ON
  -DPROTOBUF_UPDATE_FILES=OFF
  -Dprotobuf_BUILD_TESTS=OFF
)

# ===== 关键优化：静态链接第三方库 =====
echo "🔧 配置静态链接第三方库..."
STATIC_BUILD_OPTS=(
  -DBUILD_ZLIB=ON
  -DBUILD_JPEG=ON
  -DBUILD_PNG=ON
  -DBUILD_TIFF=ON
  -DBUILD_WEBP=ON
  -DBUILD_JASPER=OFF
  -DBUILD_OPENJPEG=ON
  -DCMAKE_DISABLE_FIND_PACKAGE_OpenEXR=TRUE  # 防止 CMake 查找系统 OpenEXR
)

# ===== 关键优化：模块精简 =====
echo "📦 启用精选模块..."
MODULE_OPTS=(
  -DOPENCV_ENABLE_NONFREE=OFF
  # 禁用不必要模块
  -DBUILD_opencv_python=OFF
  -DBUILD_opencv_java=OFF
  -DWITH_1394=OFF
  -DWITH_ADE=OFF
  -DWITH_VTK=OFF
  -DWITH_FFMPEG=OFF
  # 精选核心模块
  -DBUILD_LIST="core;imgproc;imgcodecs;highgui;features2d;ml;photo;dnn;objdetect;calib3d;face;xphoto;tracking;videoio;stitching;flann"
)

# 下载 OpenCV 源码
if [ ! -f "$OPENCV_SRC_DIR/CMakeLists.txt" ]; then
  echo "📥 正在下载 OpenCV 源码..."
  curl -L -o opencv.zip https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip
  unzip -q opencv.zip
  mv "opencv-${OPENCV_VERSION}" "$OPENCV_SRC_DIR"
  rm opencv.zip
else
  echo "✅ 已存在 OpenCV 源码，跳过下载。"
fi

# 下载 OpenCV contrib
if [ ! -f "$OPENCV_CONTRIB_DIR/modules/README.md" ]; then
  echo "📥 正在下载 OpenCV contrib..."
  curl -L -o opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip
  unzip -q opencv_contrib.zip
  mv "opencv_contrib-${OPENCV_VERSION}" "$OPENCV_CONTRIB_DIR"
  rm opencv_contrib.zip
else
  echo "✅ 已存在 OpenCV contrib，跳过下载。"
fi

# 清理旧构建
rm -rf "$OPENCV_BUILD_DIR"/*
cd "$OPENCV_BUILD_DIR"

# ===== 关键优化：配置 RPATH 和部署目标 =====
echo "⚙️ 正在配置 CMake（优化版）..."

cmake "$OPENCV_SRC_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="$DEPLOY_TARGET" \
  -DCMAKE_MACOSX_RPATH=ON \
  -DCMAKE_INSTALL_RPATH="@loader_path" \
  -DCMAKE_INSTALL_NAME_DIR="@rpath" \
  -DBUILD_opencv_world=ON \
  -DOPENCV_EXTRA_MODULES_PATH="$OPENCV_CONTRIB_DIR/modules" \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_TESTS=OFF \
  -DBUILD_PERF_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  "${PROTOBUF_OPTS[@]}" \
  "${STATIC_BUILD_OPTS[@]}" \
  "${EXR_OPTS[@]}" \
  "${MODULE_OPTS[@]}" \
  -DCMAKE_IGNORE_PATH="${BREW_PREFIX}/opt/openexr;${BREW_PREFIX}/opt/imath"  # 忽略 Homebrew 路径

# 编译
echo "🔨 开始编译 OpenCV..."
cmake --build . --target install -- -j$(sysctl -n hw.ncpu)

# ===== 验证输出 =====
echo "🔍 验证生成的库..."
INSTALLED_LIB="$INSTALL_DIR/lib/libopencv_world.4.10.0.dylib"

# 检查依赖项
echo "依赖项分析："
otool -L "$INSTALLED_LIB" | grep -v "$INSTALL_DIR" | grep -v "/usr/lib" | grep -v "/System"

echo "🎉 OpenCV ${OPENCV_VERSION} 构建成功！"
echo "📂 安装路径: $INSTALL_DIR"