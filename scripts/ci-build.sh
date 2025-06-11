#!/bin/bash

SCRIPT_DIR="$(dirname "$0")"
cd "$SCRIPT_DIR/../src" && make clean && make js
cd "../" && mkdir -p dist && cp -rv web/* dist/