#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

class ClearEntities : public AbstractAction {
  void act() override {
    State& localState = *this->state;
    localState.bombers.clear();
    localState.bullets.clear();
    localState.bombs.clear();
    localState.airplanes.clear();
    localState.duoMissiles.clear();
    localState.trainHeads.clear();
    localState.particles.clear();
  }

public:
  ClearEntities() {}
};

} // namespace actions

} // namespace program