# Miyoo A30 toolchain

This project has a toolchain running in docker to cross compile for the Miyoo A30. Programs using this can take advantage of SDL2. The following libraries are available

1. SDL 2.26.1
2. SDL image 2.0.1
3. SDL mixer 2.0.1
4. SDL GFX 1.0.4
5. SDL ttf 2.0.13

This is meant to work with spruceOS, but probably works with any number of firmwares.

# Usage

## Step 1 - compile the toolchain

The toolchain is built into a docker container tagged as `miyooa30-toolchain`. From the base of this project run the following

```
./build.sh
```

## Step 2 - compile the example program

The example program shows how to use SDL 2 to do basic things on the device. From the base of this project run the following

```
cd example
./build.sh
```

This will create a program `hello_sdl2` that is dynamically linked and ready to run on the Miyoo A30. You need to do the following on your SD Card

1. create a directory hello_sdl2
2. copy the `hello_sdl2` application to that directory
3. copy all the `.wav` files to that directory
4. copy the `image.bmp` file to that directory
5. copy `hello-sdl.sh` to `Roms/PORTS/`.

now put the SD Card in your A30 and boot it up. Under Games -> PORTS you should see "hello-sdl". You can run it and press the buttons to see it demonstrate different things like graphics, sound, etc.

## Step 3 - create your own program

You can look at `example/Dockerfile` to see how to build your own code to run on the Miyoo A30. Happy gaming.
