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
echo "[INFO] Scanning dependencies..."

# 过滤出非系统库
DEPS=$(otool -L "$TARGET_DYLIB" | tail -n +2 | awk '{print $1}' | grep -v "^/usr/lib" | grep -v "^/System")

for dep in $DEPS; do
    dep_filename=$(basename "$dep")
    dest_path="$DEST_DIR/$dep_filename"

    # Step 1: Copy dylib to destination
    if [ -f "$dep" ]; then
        echo "[COPY] $dep → $dest_path"
        cp -f "$dep" "$dest_path"
    else
        echo "[WARN] Dependency not found: $dep"
        continue
    fi

    # Step 2: Change dependency path inside main dylib
    echo "[FIX] Changing $dep → @loader_path/$dep_filename"
    install_name_tool -change "$dep" "@loader_path/$dep_filename" "$TARGET_DYLIB"
done

echo "[DONE] dylib dependencies copied and fixed."