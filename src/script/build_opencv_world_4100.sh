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

PROTOBUF_ROOT="${BREW_PREFIX}/opt/protobuf@21"
PROTOC_BIN="${PROTOBUF_ROOT}/bin/protoc"
if [ ! -x "$PROTOC_BIN" ]; then
  echo "❌ Error: protoc 21 not found at $PROTOC_BIN"
  echo "请先运行：brew install protobuf@21"
  exit 1
fi

echo "✅ 使用 protobuf@21 路径：$PROTOBUF_ROOT"

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

# 清理旧构建（可选）
rm -rf "$OPENCV_BUILD_DIR"/*
cd "$OPENCV_BUILD_DIR"

# 配置 CMake
echo "⚙️ 正在配置 CMake..."

cmake "$OPENCV_SRC_DIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
  -DBUILD_opencv_world=ON \
  -DOPENCV_EXTRA_MODULES_PATH="$OPENCV_CONTRIB_DIR/modules" \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_TESTS=OFF \
  -DBUILD_PERF_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_PROTOBUF=OFF \
  -DPROTOBUF_UPDATE_FILES=OFF \
  -DProtobuf_PROTOC_EXECUTABLE="$PROTOC_BIN" \
  -DProtobuf_INCLUDE_DIR="$PROTOBUF_ROOT/include" \
  -DProtobuf_LIBRARY="$PROTOBUF_ROOT/lib/libprotobuf.dylib"


# 编译
echo "🔨 开始编译 OpenCV..."
cmake --build . --target install -- -j$(sysctl -n hw.ncpu)

echo "🎉 OpenCV ${OPENCV_VERSION} 构建并安装成功！"
echo "📂 安装路径: $INSTALL_DIR"