// make testui TestHeadCollisions

#include "../setupTest.hpp"
#include "client/Render.h"
#include "game/State.h"
#include "lib/sdl2w/Animation.h"
#include "lib/sdl2w/Logger.h"
#include "utils/Timer.hpp"
#include <SDL2/SDL_timer.h>
#include <cmath>
#include <memory>

using namespace program;

int main(int argc, char** argv) {
  const int w = 640;
  const int h = 480;

  LOG(INFO) << "Start program" << LOG_ENDL;
  auto [windowPtr, storePtr] = test::setupTest(argc, argv, w, h);
  sdl2w::Window& window = *windowPtr;
  State state;

  auto _initializeLoop = [&]() {
    sdl2w::renderSplash(window);
    return true;
  };

  auto _onInitialized = [&]() {

  };

  int timeDilation = 0;

  sdl2w::Animation anim = window.getStore().createAnimation("bomber_anim");

  auto _mainLoop = [&]() {
    int dt = window.getDeltaTime();
    sdl2w::Draw& d = window.getDraw();
    d.setBackgroundColor({10, 10, 10});

    anim.update(dt);
    d.drawAnimation(anim,
                    sdl2w::RenderableParamsEx{
                        .scale = {2.0, 2.0},
                        .x = w / 2,
                        .y = h / 2,
                        .centered = true,
                    });
    d.drawSprite(window.getStore().getSprite("bomber_0"),
                 sdl2w::RenderableParamsEx{
                     .scale = {2.0, 2.0},
                     .x = w / 2,
                     .y = h / 2 + 100,
                     .centered = true,
                 });

    return true;
  };

  window.startRenderLoop(_initializeLoop, _onInitialized, _mainLoop);

  delete windowPtr;
  delete storePtr;

  sdl2w::Window::unInit();
  LOG(INFO) << "End program" << LOG_ENDL;
}