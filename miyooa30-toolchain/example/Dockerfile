FROM miyooa30-toolchain:latest

COPY show_sdl_versions.c .
RUN . /root/config.sh && $CC -std=c1x -Wall $SDL_INCLUDE -o show_sdl_versions $SDL_LIBS $SDL_LIB_PATHS -Wl,-verbose show_sdl_versions.c

# Copy in the example program
COPY hello_sdl2.c .
# Source the environmental variables and compile it
RUN . /root/config.sh &&  $CC $MARCH -std=c1x -Wall $SDL_INCLUDE -o hello_sdl2 $SDL_LIBS $SDL_LIB_PATHS hello_sdl2.c
