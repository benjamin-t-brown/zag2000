#include "lib/sdl2w/AssetLoader.h"
#include "lib/sdl2w/Draw.h"
#include "lib/sdl2w/Events.h"
#include "lib/sdl2w/Init.h"
#include "lib/sdl2w/L10n.h"
#include "lib/sdl2w/Logger.h"
#include "utils/Timer.hpp"

void runProgram(int argc, char** argv) {
  const int w = 640;
  const int h = 480;

  sdl2w::Store store;
  sdl2w::Window window(store,
                       {
                           .mode = sdl2w::DrawMode::GPU,
                           .title = "Zag 2000",
                           .w = w,
                           .h = h,
                           .x = 25, // SDL_WINDOWPOS_UNDEFINED
                           .y = 50, // SDL_WINDOWPOS_UNDEFINED
                           .renderW = w,
                           .renderH = h,
                       });
  sdl2w::L10n::init(std::vector<std::string>({})); // TODO add languages
  sdl2w::setupStartupArgs(argc, argv, window);
  sdl2w::L10n::setLanguage("default"); // TODO remove this
  window.getDraw().setBackgroundColor({0, 0, 0});
  window.getStore().loadAndStoreFont("default", "assets/monofonto.ttf");

  sdl2w::AssetLoader assetLoader(window.getDraw(), window.getStore());
  assetLoader.loadAssetsFromFile(sdl2w::ASSET_FILE, "assets/assets.txt");

  sdl2w::Draw& d = window.getDraw();
  // program::GameManager game(window);
  // game.load();
  window.setSoundPct(33);

  auto& events = window.getEvents();
  // events.setKeyboardEvent(
  //     sdl2w::ON_KEY_PRESS,
  //     [&](const std::string& key, int) { game.handleKeyPress(key); });

  auto _initializeLoop = [&]() {
    sdl2w::renderSplash(window);
    return true;
  };

  auto _onInitialized = [&]() {
    // load high scores
    // auto hiscores = hiscore::getHighScores();
    // if (hiscores.size()) {
    //   // game.state.wins = hiscores[0].score;
    // }

    // game.start();
  };

  auto _mainLoop = [&]() {
    d.setBackgroundColor({10, 10, 10});

#ifndef __EMSCRIPTEN__
    if (window.getEvents().isKeyPressed("Escape")) {
      return false;
    }
#endif

    // game.update(std::min(window.getDeltaTime(), 100));
    // game.render();
    return true;
  };

  window.startRenderLoop(_initializeLoop, _onInitialized, _mainLoop);
}

int main(int argc, char** argv) {
  LOG(INFO) << "Start program" << LOG_ENDL;
  sdl2w::Window::init();
  srand(time(NULL));

  program::Timer timer;
  runProgram(argc, argv);

  sdl2w::Window::unInit();
  LOG(INFO) << "End program" << LOG_ENDL;
}
