#!/bin/bash

set -e

if [ $# -ne 1 ]; then
    echo "Usage: $0 <path-to-your-dylib>"
    exit 1
fi

TARGET_DYLIB="$1"
DEST_DIR=$(dirname "$TARGET_DYLIB")

echo "[INFO] Target dylib: $TARGET_DYLIB"
echo "[INFO] Destination directory: $DEST_DIR"

# 用字符串记录已处理的文件名，避免 declare -A
FIXED_LIBS=""

is_fixed() {
    echo "$FIXED_LIBS" | grep -q ":$1:"
}

mark_fixed() {
    FIXED_LIBS="${FIXED_LIBS}:$1:"
}

fix_dylib() {
    local dylib="$1"
    local base=$(basename "$dylib")

    if is_fixed "$base"; then
        return
    fi

    echo "[FIXING] $base"
    mark_fixed "$base"

    # 设置 install_name
    echo "[SET-ID] install_name_tool -id @rpath/$base $dylib"
    install_name_tool -id "@rpath/$base" "$dylib"

    # 提取依赖
    local deps=$(otool -L "$dylib" | tail -n +2 | awk '{print $1}' | grep -v "^/usr/lib" | grep -v "^/System")

    for dep in $deps; do
        dep_base=$(basename "$dep")
        dep_dest="$DEST_DIR/$dep_base"

        if [ -f "$dep" ]; then
            if [ ! -f "$dep_dest" ]; then
                echo "[COPY] $dep → $dep_dest"
                cp -f "$dep" "$dep_dest"
            else
                echo "[SKIP] Already exists: $dep_dest"
            fi

            echo "[REWRITE] $dep → @loader_path/$dep_base in $dylib"
            install_name_tool -change "$dep" "@loader_path/$dep_base" "$dylib"

            fix_dylib "$dep_dest"
        else
            echo "[WARN] Dependency not found: $dep"
        fi
    done
}

fix_dylib "$TARGET_DYLIB"

echo "[DONE] All dependencies fixed recursively."
