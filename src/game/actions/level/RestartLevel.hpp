#pragma once

#include "ClearEntities.hpp"
#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/control/SetControlState.hpp"
#include "game/actions/level/CreateTrainsForLevel.hpp"
#include "game/actions/spawns/ReSpawnBush.hpp"
#include "game/actions/spawns/SpawnAirplane.hpp"
#include "game/actions/spawns/SpawnBomber.hpp"
#include "game/actions/spawns/SpawnTrain.hpp"

namespace program {

namespace actions {

class RestartLevel : public AbstractAction {
  void act() override {
    State& localState = *this->state;

    localState.controlState = CONTROL_WAITING;
    localState.player.physics.x =
        localState.playAreaXOffset +
        localState.playAreaWidthTiles * TILE_WIDTH / 2.;
    localState.player.physics.y =
        localState.playAreaBottomYStart + TILE_HEIGHT * 2 + TILE_HEIGHT / 2.;
    localState.player.physics.vx = 0;
    localState.player.physics.vy = 0;

    int playerInd = getPlayerAreaIndFromPos(
        localState, localState.player.physics.x, localState.player.physics.y);
    if (playerInd > 0) {
      // if there's a bush there
      auto it = localState.bushes.find(playerInd);
      if (it != localState.bushes.end()) {
        auto& bush = it->second;
        bush->shouldRemove = true;
      }
    }
    SpawnTrain::setAdditionalTrainSpawnTimer(localState);
    SpawnBomber::setNextBomberTimer(localState);
    SpawnAirplane::setNextAirplaneTimer(localState);

    enqueueAction(localState, nullptr, 500);
    localState.bullets.clear();

    for (auto& bushPair : localState.bushes) {
      Bush* bush = bushPair.second.get();
      if (bush->marked || bush->hp < 4) {
        auto action = actions::ReSpawnBush(bush);
        action.execute(state);
      }
    }

    enqueueAction(localState, nullptr, 1000);
    enqueueAction(localState, new actions::ClearEntities(), 0);
    enqueueAction(
        localState, new actions::CreateTrainsForLevel(localState.level), 0);
    enqueueAction(localState, nullptr, 200);
    enqueueAction(localState, new actions::SetControlState(CONTROL_IN_GAME), 0);
  }

public:
  RestartLevel() {}
};

} // namespace actions

} // namespace program