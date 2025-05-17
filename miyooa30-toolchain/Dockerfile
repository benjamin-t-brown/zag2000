FROM debian:buster-slim
ENV DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture armhf && \
  apt-get -y update  \
  && apt-get -y install \
    build-essential \
	scons \
	clang \
	sudo \
	crossbuild-essential-armhf \
	libstdc++6:armhf \
	libglib2.0-dev:armhf \
	libpng-dev:armhf \
        libfreetype6-dev:armhf \
	nano vim git curl wget unzip cmake xz-utils \
  && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /workspace; chmod +0777 /workspace; ln -s /usr/local/include /usr/include/sdkdir

WORKDIR /root
COPY cross-compile-ldd /usr/bin/arm-linux-gnueabihf-ldd
COPY setup-env.sh .
RUN cat setup-env.sh >> .bashrc
RUN echo "export MARCH='-march=armv7-a+neon-vfpv4'" > $PWD/config.sh
RUN echo "export SDL_LIBS='-lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lSDL2_image'" >> $PWD/config.sh
RUN echo "export SDL_LIB_PATHS='-L/workspace/miyooa30/usr/miyoo/lib -L/workspace/miyooa30/usr/lib -L/workspace/miyooa30/lib'" >> $PWD/config.sh
RUN echo "export SDL_INCLUDE='-I/workspace/SDL2-2.26.1/include -I/workspace/SDL2_image-2.0.1 -I/workspace/SDL2_mixer-2.0.1 -I/workspace/SDL2_gfx-1.0.4 -I/workspace/SDL2_ttf-2.0.13'" >> $PWD/config.sh
RUN chmod +x $PWD/config.sh
RUN cat $PWD/config.sh >> .bashrc

VOLUME /workspace
WORKDIR /workspace

ENV CROSS_COMPILE=/usr/bin/arm-linux-gnueabihf-
ENV PREFIX=/usr
ENV PKG_CONFIG_PATH=/usr/lib/arm-linux-gnueabihf/pkgconfig/:/usr/local/lib/pkgconfig/
ENV AS="arm-linux-gnueabihf-as"
ENV CC="arm-linux-gnueabihf-gcc"
ENV CXX="arm-linux-gnueabihf-g++"
ENV LD="arm-linux-gnueabihf-ld"

COPY SDL2-2.26.1.tar.gz .
RUN tar xf SDL2-2.26.1.tar.gz
COPY SDL2_ttf-2.0.13.tar.gz .
RUN tar xf SDL2_ttf-2.0.13.tar.gz
COPY SDL2_mixer-2.0.1.tar.gz .
RUN tar xf SDL2_mixer-2.0.1.tar.gz
COPY SDL2_image-2.0.1.tar.gz .
RUN tar xf SDL2_image-2.0.1.tar.gz
COPY SDL2_gfx-1.0.4.tar.gz .
RUN tar xf SDL2_gfx-1.0.4.tar.gz

RUN mkdir -p miyooa30
COPY lib.tar.xz miyooa30
RUN cd miyooa30/ && tar xf lib.tar.xz && rm lib.tar.xz
COPY usr.tar.xz miyooa30/ 
RUN cd miyooa30/ && tar xf usr.tar.xz && rm usr.tar.xz
RUN cd miyooa30/usr/miyoo/lib && \
ln -s libSDL2-2.0.so.0 libSDL2.so && \
ln -s libSDL2_gfx-1.0.so.0 libSDL2_gfx.so && \
ln -s libSDL2_image-2.0.so.0 libSDL2_image.so && \
ln -s libSDL2_mixer-2.0.so.0 libSDL2_mixer.so && \
ln -s libSDL2_ttf-2.0.so.0 libSDL2_ttf.so
RUN cd miyooa30/usr/lib && \
  ln -s libfreetype.so.6.11.4 libfreetype.so && \
  ln -s libbz2.so.1.0.6 libbz2.so
RUN cd miyooa30/lib && \
  ln -s libmad.so.0.2.1 libmad.so && \
  ln -s libpthread-2.23.so libpthread.so && \
  ln -s librt-2.23.so librt.so && \
  ln -s libgcc_s.so.1 libgcc_s.so
  
CMD ["/bin/bash"]
