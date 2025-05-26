#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

class SetPlayerControls : public AbstractAction {
  PlayerControls pc;

  void act() override {
    State& localState = *this->state;
    localState.player.controls = pc;
  }

public:
  SetPlayerControls(PlayerControls& pc) : pc(pc) {}
};

} // namespace actions

} // namespace program