#!/bin/bash
set -e

# 📌 配置路径
TARGET_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../build/install/lib" && pwd)"
THIRDPARTY_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../thirdparty/opencv" && pwd)"

echo "🔧 修复 dylib 路径: $TARGET_DIR"
echo "📦 使用 opencv dylib 来源: $THIRDPARTY_DIR"

# 📍 拷贝 opencv_world dylib（如果还没拷贝）
OPENCV_LIB=libopencv_world.4.10.0.dylib
if [ ! -f "$TARGET_DIR/$OPENCV_LIB" ]; then
    echo "📥 拷贝 $OPENCV_LIB 到安装目录"
    cp "$THIRDPARTY_DIR/$OPENCV_LIB" "$TARGET_DIR/"
fi

# ✅ 重命名软链接（可选，根据实际情况）
ln -sf "$OPENCV_LIB" "$TARGET_DIR/libopencv_world.410.dylib"
ln -sf "$OPENCV_LIB" "$TARGET_DIR/libopencv_world.dylib"

# 📍 主库路径
MAIN_LIB="$TARGET_DIR/libMonicaImageProcess.dylib"

# ✅ 修复主库引用的路径
echo "🔗 修复主库 $MAIN_LIB 的 opencv 引用..."
install_name_tool -change "@rpath/libopencv_world.410.dylib" "@loader_path/$OPENCV_LIB" "$MAIN_LIB"

# ✅ 修复 opencv_world 自身 ID
echo "🪪 设置 opencv_world 的 install_name..."
install_name_tool -id "@loader_path/$OPENCV_LIB" "$TARGET_DIR/$OPENCV_LIB"

# ✅ 验证依赖
echo "🔍 最终依赖分析:"
otool -L "$MAIN_LIB" | grep -E 'opencv|libMonicaImageProcess'