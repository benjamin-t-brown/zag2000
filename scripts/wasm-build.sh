#!/usr/bin/env bash
#
# Build the WASM version of ZAG2000 from MSYS2 MinGW64 bash.
#
# Prerequisites:
#   - emsdk installed at /c/progs/emsdk (with ./emsdk install latest && ./emsdk activate latest)
#   - Bash wrapper scripts for em++/emcc/emar/emranlib in emsdk/upstream/emscripten/
#
# Usage:
#   ./scripts/wasm-build.sh          # full build (sdl2w + project)
#   ./scripts/wasm-build.sh quick    # skip sdl2w rebuild
#   ./scripts/wasm-build.sh clean    # clean and rebuild everything

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SRC_DIR="$PROJECT_DIR/src"
EMSDK_DIR="/c/progs/emsdk"

export PATH="$EMSDK_DIR:$EMSDK_DIR/upstream/emscripten:$PATH"
export EMSDK="$EMSDK_DIR"
export EM_CONFIG="C:/progs/emsdk/.emscripten"
export EMSDK_PYTHON="C:/progs/emsdk/python/3.13.3_64bit/python.exe"

# Clang needs a Windows-format TEMP path, not MSYS2's /tmp
if [[ "$TEMP" == /tmp* ]] || [[ -z "$TEMP" ]]; then
  export TEMP="$(cygpath -w "$USERPROFILE" 2>/dev/null)/AppData/Local/Temp"
  export TMP="$TEMP"
fi

echo "=== WASM Build for ZAG2000 ==="
echo "Emscripten: $(em++ --version 2>&1 | head -1)"
echo ""

cd "$SRC_DIR"

if [[ "$1" == "clean" ]]; then
  echo "--- Cleaning ---"
  make clean
  echo ""
fi

if [[ "$1" != "quick" ]]; then
  echo "--- Building sdl2w for WASM ---"
  make sdl2w_wasm
  echo ""
fi

echo "--- Building ZAG2000 for WASM ---"
make TARGET=wasm -j8

echo ""
echo "--- Copying output to web/ ---"
mv ZAG2000.js "$PROJECT_DIR/web/"
mv ZAG2000.data "$PROJECT_DIR/web/"
mv ZAG2000.wasm "$PROJECT_DIR/web/"

echo ""
echo "=== Build complete ==="
ls -la "$PROJECT_DIR/web/ZAG2000."*
echo ""
echo "To test locally, serve the web/ directory with a local HTTP server:"
echo "  cd web && python3 -m http.server 8080"
