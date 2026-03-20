#!/bin/bash
set -e

SCRIPT_DIR="$(dirname "$0")"
cd "$SCRIPT_DIR/../src"
make clean
make js
cd "$SCRIPT_DIR/.."
mkdir -p dist
cp -rv web/* dist/