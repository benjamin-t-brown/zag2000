#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL2_gfxPrimitives.h>

// Window size
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

// Image size
#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

// Audio settings
#define AUDIO_RATE 48000
#define AUDIO_CHANNELS 1
#define AUDIO_CHUNKSIZE 4096
#define AUDIO_FORMAT AUDIO_S16LSB
#define AUDIO_DELAY_TICKS 2000

// font stuff
#define TTF_FILE "/mnt/SDCARD/Themes/SPRUCE/nunwen.ttf"
#define NUM_MESSAGES 15
#define FONT_SIZE 16


int main(int argc, char* args[])
{
    // messages that are displayed to the user
    const char * messages [] = {
      "up",
      "down",
      "right",
      "left",
      "home",
      "A",
      "B",
      "X",
      "Y",
      "L1",
      "L2",
      "R1",
      "R2",
      "Start",
      "Select"
    };
    // SDL initialisation
    if (SDL_Init(SDL_INIT_TIMER |
                 SDL_INIT_AUDIO |
                 SDL_INIT_VIDEO |
                 SDL_INIT_JOYSTICK |
                 SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0){
        SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Width and height are intentionally inverted, since the physical display is rotated
    SDL_Window* window = SDL_CreateWindow("Hello World SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_HEIGHT, WINDOW_WIDTH, SDL_WINDOW_SHOWN| SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
    if (window == NULL) {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Setup fonts
    if (TTF_Init() < 0) {
        SDL_Log("TTF_Init failed err: %s\n", TTF_GetError());
        return -1;
    }

    // Render creation
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Scale the Y in a manner to avoid distortion after the rotation
    SDL_RenderSetScale(renderer, 1.0, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT);
    if (! SDL_RenderTargetSupported(renderer)) {
        SDL_Log("SDL_RenderTargetSupported returns false");
        return -1;
    }
    
    // Load up a font
    TTF_Font* myfont = TTF_OpenFont(TTF_FILE, FONT_SIZE);
    if (NULL == myfont) {
        SDL_Log("TTF_OpenFont failed");
        return -1;
    }
    // create an array of textures, one for each message
    SDL_Texture ** texturesWithMessage = malloc(sizeof(SDL_Texture*) * NUM_MESSAGES);
    memset(texturesWithMessage, 0x0, sizeof(SDL_Texture*) * NUM_MESSAGES);
    
    for(int i = 0; i != NUM_MESSAGES; i++) {
        // render the message onto a surface
        SDL_Color white = {0xff,0xff,0xff};
        SDL_Surface* surfaceWithMessage = TTF_RenderText_Solid(myfont, messages[i], white);
        // convert the surface to a texture
        SDL_Texture* textureWithMessage = SDL_CreateTextureFromSurface(renderer, surfaceWithMessage);
        texturesWithMessage[i] = textureWithMessage;
        // done with the surface
        SDL_FreeSurface(surfaceWithMessage);
    }
    
    // Load the image
    SDL_Surface* image = SDL_LoadBMP("image.bmp");
    if (image == NULL) {
        SDL_Log("SDL_LoadBMP Error: %s\n", SDL_GetError());
        return -1;
    }
    // convert the image to a txture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
    if (texture == NULL) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
        return -1;
    }
    // Free the image (only the texture is used now)
    SDL_FreeSurface(image);
    
    // setup the audio system
    if (0 != Mix_OpenAudio(AUDIO_RATE, AUDIO_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNKSIZE)) {
        SDL_Log("Mix_OpenAudio failed %s\n", SDL_GetError());
        return -1;
    }
    Uint32 format = SDL_GetWindowPixelFormat(window);

    // create an array of audio chunks
    Mix_Chunk* audioChunks[NUM_MESSAGES];
    memset(audioChunks, 0x0, sizeof(Mix_Chunk*) * NUM_MESSAGES);
  
    for (int i = 0; i != NUM_MESSAGES; i++) {
        const int bufsize = 32;
        char wavfilename[bufsize];
        memset(wavfilename, 0x0, sizeof(wavfilename));
        // try and load the file, for example 'up.wav' for the one corresponding to the up button
        snprintf(wavfilename,bufsize-1,"%s.wav", messages[i]);
        audioChunks[i] = Mix_LoadWAV(wavfilename);
    }

    // create an intermediate texture with the dimensions the way the device is actually held
    SDL_Texture *intermediate = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET , WINDOW_WIDTH, WINDOW_HEIGHT);
    if (intermediate == NULL) {
        SDL_Log("SDL_CreateTexture Error: %s\n", SDL_GetError());
        return -1;
    }



    int cnt = 0;
    bool polling= true;
    int x_shift_amount = IMAGE_WIDTH / 5.0;
    int y_shift_amount = IMAGE_HEIGHT / 5.0;
    Uint64 last_audio = 0;

    // vertices used for the polygon example
    Sint16 vx[] = { 5, 75, 55 };
    Sint16 vy[] = { 5, 25, 75 };

    while (polling ) {
        int translate_x = 0;
        int translate_y = 0;
        bool show_gfx = false;

        int message_i = -1;
        // ask SDL to process any events
        SDL_PumpEvents();
        SDL_Event event;
        // check for an event
        if (1 == SDL_PollEvent(&event)) {
            // if the event is quit terminate now
            if (event.type == SDL_QUIT) {
                polling = false;
            }
        }

        int numkeys = 0;
        Uint8 const * keys = SDL_GetKeyboardState(&numkeys);

        // check each key, one at a time
        if (keys[SDL_SCANCODE_UP] ) {
            translate_y -= y_shift_amount;
            message_i = 0;
        }
        if (keys[SDL_SCANCODE_DOWN]) {
            translate_y += y_shift_amount;
            message_i = 1;
        }
        if (keys[SDL_SCANCODE_RIGHT]){
            translate_x += x_shift_amount;
            message_i = 2;
        }
        if (keys[SDL_SCANCODE_LEFT]) {
            translate_x -= x_shift_amount;
            message_i = 3;
        }
        if (keys[SDL_SCANCODE_ESCAPE]) { // 'home' button on the top
            polling = false;
            message_i = 4;
        }
        if (keys[SDL_SCANCODE_SPACE]) { // 'A' button
            message_i = 5;
        }
        if (keys[SDL_SCANCODE_LCTRL]) { // 'B' button
            message_i = 6;
        }
        if (keys[SDL_SCANCODE_LSHIFT]) { // 'X' button
            message_i = 7;
        }
        if (keys[SDL_SCANCODE_LALT]) { // 'Y' button
            message_i = 8;
        }
        if (keys[SDL_SCANCODE_E]) { // 'L1' button
            message_i = 9;
        }
        if (keys[SDL_SCANCODE_TAB]) { // 'L2' button
            message_i = 10;
        }
        if (keys[SDL_SCANCODE_T]) { // 'R1' button
            message_i = 11;
        }
        if (keys[SDL_SCANCODE_BACKSPACE]) { // 'R2' button
            message_i = 12;
        }
        if (keys[SDL_SCANCODE_RETURN]) { // 'Start' button
            message_i = 13;
            show_gfx = true;
        }
        if (keys[SDL_SCANCODE_RCTRL]) { // 'Select' button
            message_i = 14;
        }

        // The image has 256x256 and it is positioned in the middle of the screen by default
        SDL_Rect rect;
        rect.x = (int)(WINDOW_WIDTH * 0.5f - IMAGE_WIDTH * 0.5f);
        rect.y = (int)(WINDOW_HEIGHT * 0.5f - IMAGE_HEIGHT * 0.5f);
        rect.w = IMAGE_WIDTH;
        rect.h = IMAGE_HEIGHT;

        // apply translation to the image
        rect.x += translate_x;
        rect.y += translate_y;
    
        // This must be reset each iteration since filledPolygonColor sets this
        SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
        // set the target to be the intermediate texture
        if ( 0 != SDL_SetRenderTarget(renderer, intermediate)) {
            SDL_Log("SDL_SetRenderTarget fails %s\n", SDL_GetError());
            return -1;
        }
        SDL_RenderClear(renderer);
        if (show_gfx) {
            if (0 != filledPolygonColor(renderer, vx, vy, 3, 0xFFFF0000) ) {
                SDL_Log("filledPolygonColor fails %s\n", SDL_GetError());
                return -1;
            }
        } else {
            // copy the image into its position
            SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0.0, NULL, SDL_FLIP_NONE);
        }
        // check if a message should be displayed
        if (message_i >= 0) {
            // render the message on top of everything else
            SDL_RenderCopy(renderer, texturesWithMessage[message_i], NULL, NULL);
        }
        // set the target to be window itself
        SDL_SetRenderTarget(renderer, NULL);
        // clear the window
        SDL_RenderClear(renderer);
        // Copy the intermediate texture to the window, rotated 90 degrees anti clockwise. The previously
        // configured scaling prevents distortion
        SDL_RenderCopyEx(renderer, intermediate, NULL, NULL, -90.0, NULL, SDL_FLIP_NONE);
        // show the graphics to the user

        SDL_RenderPresent(renderer);
        // check if we should play audio
        if (message_i >= 0) {
            Uint64 ticks = SDL_GetTicks64();
            Uint64 elapsed = ticks - last_audio;
            Mix_Chunk * audioChunk = audioChunks[message_i];
            // make sure the audio was loaded and enough time has passed
            if (elapsed > AUDIO_DELAY_TICKS && NULL != audioChunk) {
                Mix_PlayChannel(0, audioChunk, 0);
                last_audio = ticks;
            }
        }
    
        // Limit the loop iteration speed here
        SDL_Delay(10);
        cnt++;
    }

    // Free the texture for the image
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(intermediate);
    for (int i = 0; i != NUM_MESSAGES; i++ ){
      // free the rendered message
      SDL_DestroyTexture(texturesWithMessage[i]);
      // free the audio chunk if it exists
      if (NULL != audioChunks[i]) {
          Mix_FreeChunk(audioChunks[i]);
      }
    }

    // Destroy the render, window and finalise SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
