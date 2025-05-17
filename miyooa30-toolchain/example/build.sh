#!/bin/bash

set -euo pipefail

function cleanup {
docker rm miyooa30builder
}

docker build -t miyooa30-example .
# start the container, which won't really run anything
docker run --name miyooa30builder miyooa30-example 
trap cleanup EXIT
# copy out the compiled code
docker cp miyooa30builder:/workspace/show_sdl_versions .
docker cp miyooa30builder:/workspace/hello_sdl2 .


