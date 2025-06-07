#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnTrain.hpp"
#include <cstdlib>

namespace program {

namespace actions {

class CreateTrainsForLevel : public AbstractAction {
  int level;
  void act() override {
    State& localState = *this->state;

    int trainStartX = rand() % 6 - 1;
    int trainStartY = 0;
    int numTrains = 9 + level;
    double trainSpeed = 0.15 + static_cast<double>(level) * 0.03;
    auto spawnTrain = actions::SpawnTrain(
        std::make_pair(trainStartX, trainStartY), numTrains, trainSpeed);
    spawnTrain.execute(state);

    for (int i = 0; i < level; i++) {
      TrainDirectionH hDirection = rand() % 2 == 0 ? TRAIN_RIGHT : TRAIN_LEFT;
      int xTile =
          hDirection == TRAIN_RIGHT ? -1 : localState.playAreaWidthTiles;
      int yTile = 1 + rand() % 7;

      auto spawnTrain = actions::SpawnTrain(
          std::make_pair(xTile, yTile), 0, trainSpeed + .02, hDirection);
      spawnTrain.execute(state);
    }
  }

public:
  CreateTrainsForLevel(int level) : level(level) {}
};

} // namespace actions

} // namespace program