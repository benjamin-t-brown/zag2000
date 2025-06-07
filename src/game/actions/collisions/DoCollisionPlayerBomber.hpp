#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/updaters/UpdateTrain.h"

namespace program {

namespace actions {

class DoCollisionPlayerBomber : public AbstractAction {
  Bomber* bomberPtr;

  void act() override {
    State& localState = *this->state;

    localState.player.dead = true;

    if (findBomberByPtr(localState, bomberPtr).has_value()) {
      bomberPtr->shouldRemove = true;
      enqueueAction(localState,
                    new SpawnParticle(SpawnParticle::getParticleTypeForRemove(
                                          localState.level),
                                      bomberPtr->physics.x,
                                      bomberPtr->physics.y,
                                      200),
                    0);
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
  DoCollisionPlayerBomber(Bomber* bomberPtr) : bomberPtr(bomberPtr) {}
};

} // namespace actions

} // namespace program