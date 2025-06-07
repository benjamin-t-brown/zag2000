#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/updaters/UpdateTrain.h"

namespace program {

namespace actions {

class DoCollisionPlayerTrain : public AbstractAction {
  Train* trainPtr;

  void act() override {
    State& localState = *this->state;

    localState.player.dead = true;

    if (findTrainByPtr(localState, trainPtr).has_value()) {
      trainPtr->shouldRemove = true;
      int trainInd = getTrainInd(localState, *trainPtr);
      if (trainInd > 0) {
        int x = trainInd % localState.playAreaWidthTiles;
        int y = trainInd / localState.playAreaWidthTiles;
        enqueueAction(localState,
                      new SpawnParticle(SpawnParticle::getParticleTypeForRemove(
                                            localState.level),
                                        {x, y},
                                        200),
                      0);
      }
    }
    enqueueAction(localState, new PlaySound("player_hit"), 0);

    enqueueAction(localState,
                  new SpawnParticle(
                      SpawnParticle::getParticleTypeForRemove(localState.level),
                      localState.player.physics.x,
                      localState.player.physics.y,
                      200),
                  0);
  }

public:
  DoCollisionPlayerTrain(Train* trainPtr) : trainPtr(trainPtr) {}
};

} // namespace actions

} // namespace program