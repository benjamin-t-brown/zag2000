#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/updaters/UpdateTrain.h"

namespace program {

namespace actions {

class DoCollisionBushAirplane : public AbstractAction {
  Bush* bushPtr;

  void act() override {
    State& localState = *this->state;
    int bushInd = getPlayerAreaIndFromPos(localState, bushPtr->x, bushPtr->y);
    if (localState.bushes.find(bushInd) != localState.bushes.end()) {
      bushPtr->marked = true;
    }
  }

public:
  DoCollisionBushAirplane(Bush* bush) : bushPtr(bush) {}
};

} // namespace actions

} // namespace program