#include <stdio.h>
#include <SDL.h>

const SDL_version * Mix_Linked_Version(void);
const SDL_version * TTF_Linked_Version(void);
//const SDL_version * GFX_Linked_Version(void);
const SDL_version * IMG_Linked_Version(void);

int main() {
  SDL_version sdlVer;
  SDL_GetVersion(&sdlVer);
  printf("sdl version: %u.%u.%u\n", sdlVer.major, sdlVer.minor, sdlVer.patch);
  SDL_version const * v = Mix_Linked_Version();  
  printf("mix version: %u.%u.%u\n", v->major, v->minor, v->patch); 
  
  v = TTF_Linked_Version();  
  printf("ttf version: %u.%u.%u\n", v->major, v->minor, v->patch);
  
  /**
  v = GFX_Linked_Version();  
  printf("gfx version: %u.%u.%u\n", v->major, v->minor, v->patch);
  **/
  
  v = IMG_Linked_Version();  
  printf("img version: %u.%u.%u\n", v->major, v->minor, v->patch);
   
  return 0;
  
}
