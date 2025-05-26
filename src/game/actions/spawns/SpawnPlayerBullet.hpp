#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

class SpawnPlayerBullet : public AbstractAction {
  std::pair<double, double> pos;

  void act() override {
    State& localState = *this->state;
    auto [x, y] = pos;

    localState.bullets.push_back(std::unique_ptr<Bullet>(new Bullet{
        .physics =
            {
                .x = x,
                .y = y,
                .vy = -1.,
            },
    }));
  }

public:
  SpawnPlayerBullet(std::pair<double, double> pos) : pos(pos) {}
};

} // namespace actions

} // namespace program