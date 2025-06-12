#!/bin/bash

set -e

echo "📦 打包 libopencv_world..."

# 设置变量
OPENCV_LIB_PATH="$HOME/Libs/opencv-install/lib"
BUILD_OUTPUT_DIR="/Users/Tony/CLionProjects/MonicaImageProcess/src/build/install/lib"
OPENCV_WORLD_DYLIB_NAME="libopencv_world.4.10.0.dylib"
OPENCV_IMG_HASH_DYLIB_NAME="libopencv_img_hash.4.10.0.dylib"
OPENCV_SFM_DYLIB_NAME="libopencv_sfm.4.10.0.dylib"

# 拷贝 libopencv 到构建输出目录
cp "${OPENCV_LIB_PATH}/${OPENCV_WORLD_DYLIB_NAME}" "${BUILD_OUTPUT_DIR}/"
cp "${OPENCV_LIB_PATH}/${OPENCV_IMG_HASH_DYLIB_NAME}" "${BUILD_OUTPUT_DIR}/"
cp "${OPENCV_LIB_PATH}/${OPENCV_SFM_DYLIB_NAME}" "${BUILD_OUTPUT_DIR}/"

# 使用 install_name_tool 修复 rpath
install_name_tool -id "@loader_path/${OPENCV_WORLD_DYLIB_NAME}" "${BUILD_OUTPUT_DIR}/${OPENCV_WORLD_DYLIB_NAME}"
install_name_tool -id "@loader_path/${OPENCV_IMG_HASH_DYLIB_NAME}" "${BUILD_OUTPUT_DIR}/${OPENCV_IMG_HASH_DYLIB_NAME}"
install_name_tool -id "@loader_path/${OPENCV_SFM_DYLIB_NAME}" "${BUILD_OUTPUT_DIR}/${OPENCV_SFM_DYLIB_NAME}"

# 确认是否还需要修复依赖的其他 OpenCV 模块（如果有被链接进去）
# otool -L "${BUILD_OUTPUT_DIR}/${OPENCV_DYLIB_NAME}"

echo "✅ 打包完成，路径：${BUILD_OUTPUT_DIR}/${OPENCV_DYLIB_NAME}"