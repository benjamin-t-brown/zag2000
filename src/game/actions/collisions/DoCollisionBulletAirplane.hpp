#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include <cmath>

namespace program {

namespace actions {

class DoCollisionBulletAirplane : public AbstractAction {
  Bullet* bulletPtr;
  Airplane* airplanePtr;

  void act() override {
    State& localState = *this->state;
    if (findBulletByPtr(localState, bulletPtr).has_value()) {
      bulletPtr->shouldRemove = true;
    }
    if (findAirplaneByPtr(localState, airplanePtr).has_value()) {
      int score = 1000;
      localState.player.score += score;
      airplanePtr->shouldRemove = true;

      enqueueAction(
          localState,
          new actions::SpawnParticle(
              std::to_string(score), airplanePtr->x, airplanePtr->y, 1000),
          0);
      enqueueAction(localState, new PlaySound("hit"), 0);
    }
  }

public:
  DoCollisionBulletAirplane(Bullet* bullet, Airplane* airplane)
      : bulletPtr(bullet), airplanePtr(airplane) {}
};

} // namespace actions

} // namespace program