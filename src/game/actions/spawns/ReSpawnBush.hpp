#pragma once

#include "SpawnParticle.hpp"
#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"
#include "game/updaters/UpdateTrain.h"
#include <cstdlib>

namespace program {

namespace actions {

class ResetBush : public AbstractAction {
  Bush* bushPtr;

  void act() override {
    bushPtr->hp = 4;
    bushPtr->marked = false;
    bushPtr->shouldRemove = false;
    state->player.score += 5;
  }

public:
  ResetBush(Bush* bushPtr) : bushPtr(bushPtr) {}
};

class ReSpawnBush : public AbstractAction {
  static int soundCtr;

  Bush* bushPtr;
  void act() override {
    State& localState = *this->state;

    int bushInd = getPlayerAreaIndFromPos(localState, bushPtr->x, bushPtr->y);
    if (localState.bushes.find(bushInd) != localState.bushes.end()) {
      int x = bushInd % localState.playAreaWidthTiles;
      int y = bushInd / localState.playAreaWidthTiles;
      enqueueAction(localState, new actions::ResetBush(bushPtr), 0);
      if (soundCtr++ % 2 == 0) {
        enqueueAction(localState, new actions::PlaySound("hit"), 0);
      } else {
        enqueueAction(localState, new actions::PlaySound("hit2"), 0);
      }
      enqueueAction(localState,
                    new SpawnParticle(SpawnParticle::getParticleTypeForRemove(
                                          localState.level),
                                      {x, y},
                                      200),
                    100);
    }
  }

public:
  ReSpawnBush(Bush* bushPtr) : bushPtr(bushPtr) {}
};

inline int ReSpawnBush::soundCtr = 0;

} // namespace actions

} // namespace program