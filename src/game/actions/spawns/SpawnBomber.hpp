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
                                  localState.playAreaWidthTiles * TILE_WIDTH
                            : localState.playAreaXOffset;
    bomber->physics.y = localState.playAreaBottomYStart;
    bomber->physics.friction = 0.008;
    bomber->speed = 0.0011;
    bomber->heading.rotationRate = 0.35;
    timer::start(bomber->shootTimer, 750);
    setNewWalkTarget(*bomber, localState);

    localState.bombers.push_back(std::unique_ptr<Bomber>(bomber));
  }

public:
  static void setNewWalkTarget(Bomber& bomber, State& state) {
    bomber.walkX =
        rand() % state.playAreaWidthTiles * TILE_WIDTH + state.playAreaXOffset;
    bomber.walkY = rand() % 7 + state.playAreaBottomYStart;
  }
  static void setNextBomberTimer(State& state) {
    int ms = 3000 + rand() % 8000;
    timer::start(state.bomberSpawnTimer, ms);
  }
  SpawnBomber(bool isRightSide) : isRightSide(isRightSide) {}
};

} // namespace actions

} // namespace program