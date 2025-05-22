#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

class SpawnLevelBushes : public AbstractAction {
  void act() override {
    State& localState = *this->state;

    int numBushes = 38;

    int leftBound = 1;
    int rightBound = localState.playAreaWidthTiles - 2;
    int upperBound = 1;
    int lowerBound = localState.playAreaHeightTiles - 5;

    for (int i = 0; i < numBushes; i++) {
      int xTile = rand() % (rightBound - leftBound) + leftBound;
      int yTile = rand() % (lowerBound - upperBound) + upperBound;

      // find if bush is in state already
      int bushInd = xTile + yTile * localState.playAreaWidthTiles;
      if (localState.bushes.find(bushInd) != localState.bushes.end()) {
        i--;
        continue;
      }

      // check if xTile and yTile are in bounds
      if (xTile < 0 || xTile >= localState.playAreaWidthTiles || yTile < 0 ||
          yTile >= localState.playAreaHeightTiles) {
        continue;
      }

      Bush* bush = new Bush();
      bush->x = xTile * TILE_WIDTH + localState.playAreaXOffset + TILE_WIDTH / 2.;
      bush->y =
          yTile * TILE_HEIGHT + localState.playAreaYOffset + TILE_HEIGHT / 2.;
      bush->hp = 4;
      bush->marked = false;
      localState.bushes[bushInd] = std::unique_ptr<Bush>(bush);
    }
  }
};

} // namespace actions

} // namespace program