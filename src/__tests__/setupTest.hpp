#include "lib/sdl2w/AssetLoader.h"
#include "lib/sdl2w/Draw.h"
#include "lib/sdl2w/Init.h"
#include "lib/sdl2w/L10n.h"
#include "lib/sdl2w/Window.h"

namespace test {
inline std::pair<sdl2w::Window*, sdl2w::Store*>
setupTest(int argc, char** argv, int w, int h) {
  sdl2w::Window::init();
  srand(time(NULL));

  sdl2w::Store* store = new sdl2w::Store();
  sdl2w::Window* window =
      new sdl2w::Window(*store,
                        {
                            .mode = sdl2w::DrawMode::GPU,
                            .title = "Zag 2000 Test",
                            .w = w,
                            .h = h,
                            .x = 25, // SDL_WINDOWPOS_UNDEFINED
                            .y = 50, // SDL_WINDOWPOS_UNDEFINED
                            .renderW = w,
                            .renderH = h,
                        });
  sdl2w::L10n::init(std::vector<std::string>({}));
  sdl2w::setupStartupArgs(argc, argv, *window);
  sdl2w::L10n::setLanguage("default");
  window->getDraw().setBackgroundColor({0, 0, 0});
  window->getStore().loadAndStoreFont("default", "assets/monofonto.ttf");
  sdl2w::AssetLoader assetLoader(window->getDraw(), window->getStore());
  assetLoader.loadAssetsFromFile(sdl2w::ASSET_FILE, "assets/assets.txt");
  window->setSoundPct(33);

  return std::make_pair(window, store);
}
} // namespace test