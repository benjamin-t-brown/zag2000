#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

class SpawnBomber : public AbstractAction {
  bool isRightSide;

  void act() override {
    State& localState = *this->state;

    Bomber* bomber = new Bomber();
    bomber->physics.x = isRightSide
                            ? localState.playAreaXOffset +
                                  localState.playAreaWidthTiles * TILE_WIDTH -
                                  TILE_WIDTH / 2.
                            : localState.playAreaXOffset + TILE_WIDTH / 2.;
    bomber->physics.y = localState.playAreaBottomYStart;
    bomber->physics.friction = 0.008;
    bomber->speed = 0.0015;
    bomber->heading.rotationRate = 0.35;
    setNewWalkTarget(*bomber, localState);

    localState.bombers.push_back(std::unique_ptr<Bomber>(bomber));
  }

public:
  static void setNewWalkTarget(Bomber& bomber, State& state) {
    bomber.walkX =
        rand() % state.playAreaWidthTiles * TILE_WIDTH + state.playAreaXOffset;
    bomber.walkY = rand() % 7 + state.playAreaBottomYStart;
  }
  SpawnBomber(bool isRightSide) : isRightSide(isRightSide) {}
};

} // namespace actions

} // namespace program