#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include <cstdlib>

namespace program {

namespace actions {

class SpawnAirplane : public AbstractAction {
  TrainDirectionH hDirection;

  void act() override {
    State& localState = *this->state;

    Airplane* airplane = new Airplane();
    airplane->x = hDirection == TRAIN_LEFT
                      ? localState.playAreaXOffset +
                            localState.playAreaWidthTiles * TILE_WIDTH
                      : localState.playAreaXOffset;
    airplane->y = TILE_HEIGHT * (2 + rand() % (10)) +
                  localState.playAreaYOffset - TILE_HEIGHT_D / 2.;
    airplane->vx = hDirection == TRAIN_LEFT ? -0.251 : 0.251;

    localState.airplanes.push_back(std::unique_ptr<Airplane>(airplane));
  }

public:
  static void setNextAirplaneTimer(State& state) {
    int ms = 10000 + rand() % 10000;
    timer::start(state.airplaneSpawnTimer, ms);
  }
  SpawnAirplane(TrainDirectionH hDirection) : hDirection(hDirection) {}
};

} // namespace actions

} // namespace program