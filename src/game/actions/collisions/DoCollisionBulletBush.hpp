#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/updaters/UpdateTrain.h"

namespace program {

namespace actions {

class DoCollisionBulletBush : public AbstractAction {
  Bullet* bulletPtr;
  Bush* bushPtr;

  void act() override {
    State& localState = *this->state;
    if (findBulletByPtr(localState, bulletPtr).has_value()) {
      bulletPtr->shouldRemove = true;
    }
    int bushInd = getPlayerAreaIndFromPos(localState, bushPtr->x, bushPtr->y);
    if (localState.bushes.find(bushInd) != localState.bushes.end()) {
      bushPtr->shouldRemove = true;
      bushPtr->hp -= 1;
      if (bushPtr->hp <= 0) {
        bushPtr->shouldRemove = true;
        localState.score += 10;
      }
    }
  }

public:
  DoCollisionBulletBush(Bullet* bulletPtr, Bush* bushPtr)
      : bulletPtr(bulletPtr), bushPtr(bushPtr) {}
};

} // namespace actions

} // namespace program