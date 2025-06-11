#pragma once

#include "ClearEntities.hpp"
#include "StartNextLevel.hpp"
#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnAirplane.hpp"
#include "game/actions/spawns/SpawnBomber.hpp"
#include "game/actions/spawns/SpawnDuoMissile.hpp"
#include "game/actions/spawns/SpawnLevelBushes.hpp"
#include "game/actions/ui/PlaySound.hpp"

namespace program {

namespace actions {

class StartGame : public AbstractAction {
  void act() override {
    State& localState = *this->state;
    actions::ClearEntities clearEntitiesAction;
    clearEntitiesAction.execute(&localState);
    localState.bushes.clear();

    localState.player.physics.x =
        localState.playAreaXOffset +
        localState.playAreaWidthTiles * TILE_WIDTH / 2.;
    localState.player.physics.y =
        localState.playAreaBottomYStart + TILE_HEIGHT * 2 + TILE_HEIGHT / 2.;
    localState.player.physics.vx = 0;
    localState.player.physics.vy = 0;
    localState.player.lives = 0;
    localState.player.score = 0;
    localState.player.dead = false;
    localState.numExtraLives = 0;

    for (int i = 0; i < localState.playAreaWidthTiles; i++) {
      for (int j = 0; j < localState.playAreaHeightTiles; j++) {
        localState.bg.push_back(rand() % 10);
      }
    }

    SpawnTrain::setAdditionalTrainSpawnTimer(localState);
    SpawnBomber::setNextBomberTimer(localState);
    SpawnAirplane::setNextAirplaneTimer(localState);
    SpawnDuoMissile::setNextDuoMissileTimer(localState);

    enqueueAction(
        localState, new actions::StartNextLevel(1, CONTROL_IN_GAME), 0);
    enqueueAction(localState, new actions::SpawnLevelBushes(), 0);
    enqueueAction(localState, new actions::PlaySound("start_game"), 0);
  }

public:
  StartGame() {}
};

} // namespace actions

} // namespace program