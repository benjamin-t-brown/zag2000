#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"

namespace program {

namespace actions {

class SpawnCollisionCircle : public AbstractAction {
  std::pair<double, double> pos;

  void act() override {
    State& localState = *this->state;

    CollisionCircle* circle = new CollisionCircle();
    circle->timer = Timer{static_cast<double>(100), 0};
    circle->x = pos.first;
    circle->y = pos.second;
    circle->radius = TILE_WIDTH;

    localState.collisionCircles.push_back(
        std::unique_ptr<CollisionCircle>(circle));
    enqueueAction(localState, new PlaySound("bomb_expl"), 0);
  }

public:
  SpawnCollisionCircle(std::pair<double, double> pos) : pos(pos) {}
};

} // namespace actions

} // namespace program