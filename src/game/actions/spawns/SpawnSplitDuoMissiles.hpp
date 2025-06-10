#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"
#include <cstdlib>

namespace program {

namespace actions {

class SpawnSplitDuoMissiles : public AbstractAction {
  int x;
  int y;
  void act() override {
    State& localState = *this->state;

    const double speed = 0.521;
    const int turnTime = 50 + rand() % 150;

    {
      DuoMissile* missile = new DuoMissile();
      missile->type = DUO_MISSILE_SINGLE;
      missile->physics.x = x;
      missile->physics.y = y;
      missile->physics.vx = speed;
      missile->physics.friction = 0.00;
      timer::start(missile->turnTimer, turnTime);
      localState.duoMissiles.push_back(std::unique_ptr<DuoMissile>(missile));
    }
    {
      DuoMissile* missile = new DuoMissile();
      missile->type = DUO_MISSILE_SINGLE;
      missile->physics.x = x;
      missile->physics.y = y;
      missile->physics.vx = -speed;
      missile->physics.friction = 0.00;
      timer::start(missile->turnTimer, turnTime);
      localState.duoMissiles.push_back(std::unique_ptr<DuoMissile>(missile));
    }

    enqueueAction(localState, new PlaySound("missile_split"), 0);
    addParallelAction(
        localState,
        std::unique_ptr<PlaySound>(new PlaySound("missile_speed")),
        200);
  }

public:
  SpawnSplitDuoMissiles(int x, int y) : x(x), y(y) {}
};

} // namespace actions

} // namespace program