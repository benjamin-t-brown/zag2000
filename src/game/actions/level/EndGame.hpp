#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/control/SetControlState.hpp"
#include "game/actions/spawns/ReSpawnBush.hpp"
#include "game/actions/ui/PlaySound.hpp"

namespace program {

namespace actions {

class EndGame : public AbstractAction {
  void act() override {
    State& localState = *this->state;

    localState.controlState = CONTROL_WAITING;

    for (auto& bushPair : localState.bushes) {
      Bush* bush = bushPair.second.get();
      if (bush->marked || bush->hp < 4) {
        auto action = actions::ReSpawnBush(bush);
        action.execute(state);
      }
    }

    enqueueAction(localState, new actions::PlaySound("game_over"), 0);
    enqueueAction(localState,
                  new actions::SetControlState(CONTROL_SHOWING_HIGH_SCORE),
                  200);
  }

public:
  EndGame() {}
};

} // namespace actions

} // namespace program