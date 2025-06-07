#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"

namespace program {

namespace actions {

class DoCollisionPlayerCollisionCircle : public AbstractAction {

  void act() override {
    State& localState = *this->state;

    localState.player.dead = true;

    enqueueAction(localState, new PlaySound("player_hit"), 0);

    enqueueAction(localState,
                  new SpawnParticle(
                      SpawnParticle::getParticleTypeForRemove(localState.level),
                      localState.player.physics.x,
                      localState.player.physics.y,
                      200),
                  0);
  }
};

} // namespace actions

} // namespace program