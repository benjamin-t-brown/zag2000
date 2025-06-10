#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "game/updaters/UpdateTrain.h"

namespace program {

namespace actions {

class DoCollisionPlayerDuoMissile : public AbstractAction {
  DuoMissile* missilePtr;

  void act() override {
    State& localState = *this->state;

    localState.player.dead = true;

    if (findDuoMissileByPtr(localState, missilePtr).has_value()) {
      missilePtr->shouldRemove = true;
      enqueueAction(localState, new PlaySound("player_hit"), 0);
    }

    enqueueAction(localState,
                  new SpawnParticle(
                      SpawnParticle::getParticleTypeForRemove(localState.level),
                      localState.player.physics.x,
                      localState.player.physics.y,
                      200),
                  0);
  }

public:
  DoCollisionPlayerDuoMissile(DuoMissile* missilePtr)
      : missilePtr(missilePtr) {}
};

} // namespace actions

} // namespace program