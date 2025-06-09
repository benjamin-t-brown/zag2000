#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/updaters/UpdateTrain.h"

namespace program {

namespace actions {

class DoCollisionBushCollisionCircle : public AbstractAction {
  Bush* bushPtr;

  void act() override {
    State& localState = *this->state;
    int bushInd = getPlayerAreaIndFromPos(localState, bushPtr->x, bushPtr->y);
    if (localState.bushes.find(bushInd) != localState.bushes.end()) {
      bushPtr->shouldRemove = true;
    }
  }

public:
  DoCollisionBushCollisionCircle(Bush* bush) : bushPtr(bush) {}
};

} // namespace actions

} // namespace program