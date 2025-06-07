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

    localState.soundsToPlay.push_back(soundName);
  }

public:
  PlaySound(std::string_view soundName) : soundName(soundName) {}
};

} // namespace actions

} // namespace program