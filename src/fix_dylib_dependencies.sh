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
    if [ -f "$dep" ]; then
        echo "[COPY] $dep → $DEST_DIR"
        cp -f "$dep" "$DEST_DIR"
    else
        echo "[WARN] Dependency not found: $dep"
    fi
done

echo "[DONE] dylib dependencies copied."