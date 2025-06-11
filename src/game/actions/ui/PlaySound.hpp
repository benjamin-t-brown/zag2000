#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include <cstdlib>

namespace program {

namespace actions {

class PlaySound : public AbstractAction {
  std::string soundName;

  void act() override {
    State& localState = *this->state;

    if (localState.controlState == CONTROL_MENU || localState.controlState == CONTROL_WAITING_MENU) {
      if (soundName != "start_game") {
        return;
      }
    }

    localState.soundsToPlay.push_back(soundName);
  }

public:
  PlaySound(std::string_view soundName) : soundName(soundName) {}
};

} // namespace actions

} // namespace program