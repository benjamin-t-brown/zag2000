#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
echo "SCRIPT_DIR: $SCRIPT_DIR"
cd "$SCRIPT_DIR/../src"
make clean
make js
cd "$SCRIPT_DIR/.."
mkdir -p dist
cp -rv web/* dist/