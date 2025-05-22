#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

class SpawnBush : public AbstractAction {
  std::pair<int, int> tilePos;

  void act() override {
    State& localState = *this->state;

    auto [xTile, yTile] = tilePos;

    // find if bush is in state already
    int bushInd = xTile + yTile * localState.playAreaWidthTiles;
    if (localState.bushes.find(bushInd) != localState.bushes.end()) {
      return;
    }

    // check if xTile and yTile are in bounds
    if (xTile < 0 || xTile >= localState.playAreaWidthTiles || yTile < 0 ||
        yTile >= localState.playAreaHeightTiles) {
      return;
    }

    Bush* bush = new Bush();
    bush->x = xTile * TILE_WIDTH + localState.playAreaXOffset + TILE_WIDTH / 2.;
    bush->y =
        yTile * TILE_HEIGHT + localState.playAreaYOffset + TILE_HEIGHT / 2.;
    bush->hp = 4;
    bush->marked = false;
    localState.bushes[bushInd] = std::unique_ptr<Bush>(bush);
  }

public:
  SpawnBush(std::pair<int, int> tilePos) : tilePos(tilePos) {}
};

} // namespace actions

} // namespace program