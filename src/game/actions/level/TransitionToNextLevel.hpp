#pragma once

#include "StartNextLevel.hpp"
#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include <cstdlib>

namespace program {

namespace actions {

class TransitionToNextLevel : public AbstractAction {
  void act() override {
    State& localState = *this->state;

    enqueueAction(localState, new actions::StartNextLevel(localState.level + 1), 0);
  }

public:
  TransitionToNextLevel() {}
};

} // namespace actions

} // namespace program