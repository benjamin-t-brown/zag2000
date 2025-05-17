#!/bin/bash

docker run --rm -it --mount type=bind,source="$(cygpath -w "$(pwd)")/../src",target=/workspace/src --entrypoint bash miyooa30-toolchain
docker image prune -f
