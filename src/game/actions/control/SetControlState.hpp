#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

class SetControlState : public AbstractAction {
  ControlState controlState;
  void act() override {
    State& localState = *this->state;

    localState.controlState = controlState;
    localState.controlIsWaitingForGameOver = false;
    if (controlState == CONTROL_IN_GAME) {
      localState.player.dead = false;
    }
  }

public:
  SetControlState(ControlState controlState) : controlState(controlState) {}
};

} // namespace actions

} // namespace program