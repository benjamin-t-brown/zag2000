#pragma once

#include "ClearEntities.hpp"
#include "StartNextLevel.hpp"
#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnAirplane.hpp"

namespace program {

namespace actions {

class StartGame : public AbstractAction {
  void act() override {
    State& localState = *this->state;
    actions::ClearEntities clearEntitiesAction;
    clearEntitiesAction.execute(&localState);
    localState.bushes.clear();

    enqueueAction(localState, new actions::StartNextLevel(1), 0);
    enqueueAction(localState, new actions::SpawnLevelBushes(), 0);
    SpawnTrain::setAdditionalTrainSpawnTimer(localState);
    SpawnBomber::setNextBomberTimer(localState);
    SpawnAirplane::setNextAirplaneTimer(localState);
  }

public:
  StartGame() {}
};

} // namespace actions

} // namespace program