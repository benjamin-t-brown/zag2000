#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

class SpawnPlayerBullet : public AbstractAction {
  std::pair<int, int> pos;

  void act() override {
    State& localState = *this->state;

    auto [x, y] = pos;
  }

public:
  SpawnPlayerBullet(std::pair<int, int> pos) : pos(pos) {}
};

} // namespace actions

} // namespace program