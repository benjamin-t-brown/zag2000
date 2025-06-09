#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include <cmath>

namespace program {

namespace actions {

class DoCollisionBulletBomber : public AbstractAction {
  Bullet* bulletPtr;
  Bomber* bomberPtr;

  void act() override {
    State& localState = *this->state;
    if (findBulletByPtr(localState, bulletPtr).has_value()) {
      bulletPtr->shouldRemove = true;
    }
    if (findBomberByPtr(localState, bomberPtr).has_value()) {
      bomberPtr->shouldRemove = true;

      const int distFromPlayer = static_cast<int>(std::sqrt(
          std::pow(bomberPtr->physics.x - localState.player.physics.x, 2) +
          std::pow(bomberPtr->physics.y - localState.player.physics.y, 2)));

      int score = 300;
      if (distFromPlayer < TILE_WIDTH) {
        score = 900;
      } else if (distFromPlayer < TILE_WIDTH * 2) {
        score = 600;
      }
      enqueueAction(
          localState,
          new SpawnParticle(
              SpawnParticle::getParticleTypeForRemove(localState.level),
              {bomberPtr->physics.x, bomberPtr->physics.y},
              200),
          0);
      enqueueAction(localState,
                    new actions::SpawnParticle(std::to_string(score),
                                               bomberPtr->physics.x,
                                               bomberPtr->physics.y,
                                               1000),
                    0);
      enqueueAction(localState, new PlaySound("hit"), 0);

      localState.player.score += score;
    }
  }

public:
  DoCollisionBulletBomber(Bullet* bullet, Bomber* bomber)
      : bulletPtr(bullet), bomberPtr(bomber) {}
};

} // namespace actions

} // namespace program