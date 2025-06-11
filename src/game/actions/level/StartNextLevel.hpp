#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/control/SetControlState.hpp"
#include "game/actions/level/CreateTrainsForLevel.hpp"
#include "game/actions/spawns/SpawnBush.hpp"
#include <cstdlib>

namespace program {

namespace actions {

class StartNextLevel : public AbstractAction {
  int level;
  ControlState nextControlState;
  void act() override {
    State& localState = *this->state;

    localState.level = level;
    localState.controlState = CONTROL_WAITING;

    int floatingBushX = rand() % (localState.playAreaWidthTiles - 4) + 2;
    int floatingBushY = 0;

    localState.player.score += 1000 + localState.level * 100;

    enqueueAction(
        localState,
        new actions::SpawnBush(std::make_pair(floatingBushX, floatingBushY)),
        0);
    enqueueAction(
        localState, new actions::CreateTrainsForLevel(localState.level), 0);
    enqueueAction(
        localState, new actions::SetControlState(nextControlState), 0);
  }

public:
  StartNextLevel(int level, ControlState nextControlState)
      : level(level), nextControlState(nextControlState) {}
};

} // namespace actions

} // namespace program