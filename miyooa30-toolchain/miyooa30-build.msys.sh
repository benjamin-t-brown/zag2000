#!/bin/bash

docker run --rm -it --mount type=bind,source="$(cygpath -w "$(pwd)")/../src",target=/workspace/src miyooa30-toolchain bash -c "cd /workspace/src && source ~/config.sh && make clean && make sdl2w_miyooa30 && make -j8 miyooa30"
docker image prune -f
rm -rf ScoundrelGame.miyooa30.tar.gz
tar -czf ScoundrelGame.miyooa30.tar.gz -C ../src/dist --transform 's,^,ScoundrelGame/,' .