#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"

namespace program {

namespace actions {

class SpawnBomb : public AbstractAction {
  std::pair<double, double> startPos;
  std::pair<double, double> endPos;

  void act() override {
    State& localState = *this->state;

    Bomb* bomb = new Bomb();
    bomb->x = startPos.first;
    bomb->y = startPos.second;
    bomb->x2 = endPos.first;
    bomb->y2 = endPos.second;
    bomb->transformTimer = Timer{750, 0};

    localState.bombs.push_back(std::unique_ptr<Bomb>(bomb));

    enqueueAction(localState, new actions::PlaySound("shoot_bomb"), 0);
  }

public:
  static void setNewWalkTarget(Bomber& bomber, State& state) {
    bomber.walkX =
        rand() % state.playAreaWidthTiles * TILE_WIDTH + state.playAreaXOffset;
    bomber.walkY = rand() % 7 + state.playAreaBottomYStart;
  }
  SpawnBomb(std::pair<double, double> startPos,
            std::pair<double, double> endPos)
      : startPos(startPos), endPos(endPos) {}
};

} // namespace actions

} // namespace program