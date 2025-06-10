#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnTrain.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace program {

namespace actions {

class CreateTrainsForLevel : public AbstractAction {
  int level;
  void act() override {
    State& localState = *this->state;

    int trainStartX = rand() % 6 - 1;
    int trainStartY = 0;
    int numTrains = 8 + level / 2;
    double trainSpeed = 0.15 + static_cast<double>(level) * 0.03;
    auto spawnTrain = actions::SpawnTrain(
        std::make_pair(trainStartX, trainStartY), numTrains, trainSpeed);
    spawnTrain.execute(state);

    for (int i = 0; i < std::min(level, 10); i++) {
      TrainDirectionH hDirection = rand() % 2 == 0 ? TRAIN_RIGHT : TRAIN_LEFT;
      int xTile =
          hDirection == TRAIN_RIGHT ? -1 : localState.playAreaWidthTiles;
      int yTile = 1 + rand() % 7;
      auto pos = std::make_pair(xTile, yTile);

      bool skipSpawn = false;

      for (auto& trainHead : localState.trainHeads) {
        double dist = std::sqrt(
            std::pow(trainHead->x -
                         (xTile * TILE_WIDTH + localState.playAreaXOffset),
                     2) +
            std::pow(trainHead->y -
                         (yTile * TILE_HEIGHT + localState.playAreaYOffset),
                     2));
        if (dist < 100) {
          skipSpawn = true;
          break;
        }
      }

      if (skipSpawn) {
        continue;
      }
      auto spawnTrain =
          actions::SpawnTrain(pos, 0, trainSpeed + .02, hDirection);
      spawnTrain.execute(state);
    }
  }

public:
  CreateTrainsForLevel(int level) : level(level) {}
};

} // namespace actions

} // namespace program