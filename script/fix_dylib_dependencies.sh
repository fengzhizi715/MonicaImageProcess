#!/bin/bash
set -e

# 📌 计算路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
TARGET_DIR="${PROJECT_ROOT}/build/install/lib"
OPENCV_DIR="${PROJECT_ROOT}/thirdparty/opencv-install/lib"

# ✅ 配置 opencv_world 实际文件名
OPENCV_LIB_NAME="libopencv_world.4.10.0.dylib"
OPENCV_SRC_PATH="${OPENCV_DIR}/${OPENCV_LIB_NAME}"
OPENCV_DST_PATH="${TARGET_DIR}/${OPENCV_LIB_NAME}"

echo "🔧 修复 dylib 路径"
echo "📍 主库目录: $TARGET_DIR"
echo "📦 OpenCV 源: $OPENCV_SRC_PATH"

# ✅ 检查并拷贝 opencv_world.dylib
if [ ! -f "$OPENCV_DST_PATH" ]; then
    echo "📥 拷贝 $OPENCV_LIB_NAME 到安装目录"
    cp "$OPENCV_SRC_PATH" "$OPENCV_DST_PATH"
else
    echo "✅ OpenCV 库已存在，跳过拷贝"
fi

# ✅ 清理历史软链接，避免循环引用
echo "🧹 清理历史软链接..."
find "$TARGET_DIR" -type l -name "libopencv_world*.dylib" -exec rm -f {} \;

# ✅ 创建新软链接（都指向实际文件）
echo "🔗 创建软链接..."
ln -s "$OPENCV_LIB_NAME" "$TARGET_DIR/libopencv_world.dylib"
ln -s "$OPENCV_LIB_NAME" "$TARGET_DIR/libopencv_world.410.dylib"

# 📍 主库路径
MAIN_LIB="$TARGET_DIR/libMonicaImageProcess.dylib"
if [ ! -f "$MAIN_LIB" ]; then
    echo "❌ 错误: 找不到主库 $MAIN_LIB"
    exit 1
fi

# ✅ 修复主库引用的路径
echo "🔧 修复主库 $MAIN_LIB 的 opencv 引用..."
install_name_tool -change "@rpath/libopencv_world.410.dylib" "@loader_path/${OPENCV_LIB_NAME}" "$MAIN_LIB"

# ✅ 设置 opencv_world 的 install_name
echo "🪪 设置 opencv_world 的 install_name..."
install_name_tool -id "@loader_path/${OPENCV_LIB_NAME}" "$OPENCV_DST_PATH"

# ✅ 最终验证
echo "🔍 最终依赖分析:"
otool -L "$MAIN_LIB" | grep -E 'opencv|libMonicaImageProcess'