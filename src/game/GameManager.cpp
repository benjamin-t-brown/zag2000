#include "GameManager.h"
#include "lib/sdl2w/Logger.h"
#include "lib/sdl2w/Window.h"

namespace program {
GameManager::GameManager(sdl2w::Window& windowA)
    : window(windowA), r(windowA) {}

void GameManager::load() {
  r.setup();
}

void GameManager::start() {}

void GameManager::handleKeyPress(const std::string& key) {}

void GameManager::update(int dt) {}

void GameManager::render() {}

} // namespace program