#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/actions/spawns/SpawnSplitDuoMissiles.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include <cmath>

namespace program {

namespace actions {

class DoCollisionBulletDuoMissile : public AbstractAction {
  Bullet* bulletPtr;
  DuoMissile* missilePtr;

  void act() override {
    State& localState = *this->state;
    if (findBulletByPtr(localState, bulletPtr).has_value()) {
      bulletPtr->shouldRemove = true;
    }
    if (findDuoMissileByPtr(localState, missilePtr).has_value()) {
      int score = 200;
      localState.player.score += score;
      missilePtr->shouldRemove = true;
      if (missilePtr->type == DUO_MISSILE_COMBINED) {
        enqueueAction(localState,
                      new SpawnSplitDuoMissiles(missilePtr->physics.x,
                                                missilePtr->physics.y),
                      0);
      } else if (missilePtr->type == DUO_MISSILE_SINGLE) {
        score *= 2;
      }

      enqueueAction(localState,
                    new actions::SpawnParticle(std::to_string(score),
                                               missilePtr->physics.x,
                                               missilePtr->physics.y,
                                               1000),
                    0);
      enqueueAction(localState, new PlaySound("hit"), 0);
    }
  }

public:
  DoCollisionBulletDuoMissile(Bullet* bullet, DuoMissile* missile)
      : bulletPtr(bullet), missilePtr(missile) {}
};

} // namespace actions

} // namespace program