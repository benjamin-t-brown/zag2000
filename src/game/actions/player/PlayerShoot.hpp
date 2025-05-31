#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnPlayerBullet.hpp"

namespace program {

namespace actions {

class PlayerShoot : public AbstractAction {
  void act() override {
    State& localState = *this->state;

    Player& player = localState.player;

    if (!player.canShoot) {
      return;
    }

    player.canShoot = false;
    timer::start(player.shootTimer);
    enqueueAction(
        localState,
        new actions::SpawnPlayerBullet(physics::getPos(player.physics)),
        0);
  }
};

} // namespace actions

} // namespace program