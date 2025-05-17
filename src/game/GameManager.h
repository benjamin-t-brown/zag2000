#pragma once

#include "client/Render.h"
#include "game/actions/AbstractAction.h"
#include "State.h"
#include <memory>
#include <string>

namespace program {

class GameManager {
  State state;
  sdl2w::Window& window;
  Render r;
  std::string lastKeyPressed;

  std::vector<std::unique_ptr<actions::AbstractAction>> persistentActions;

public:
  GameManager(sdl2w::Window& windowA);
  ~GameManager();

  void load();
  void start();
  void handleKeyPress(const std::string& key);
  void update(int dt);
  void render();
};
} // namespace program