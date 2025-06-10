#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/ui/PlaySound.hpp"
#include <cstdlib>

namespace program {

namespace actions {

class SpawnDuoMissile : public AbstractAction {
  void act() override {
    State& localState = *this->state;

    DuoMissile* missile = new DuoMissile();
    missile->type = DUO_MISSILE_COMBINED;
    missile->physics.x =
        localState.playAreaXOffset +
        (2 + (rand() % (localState.playAreaWidthTiles - 4))) * TILE_WIDTH_D +
        TILE_WIDTH_D / 2.;
    missile->physics.y = 0;
    missile->physics.vy = 0.251;
    missile->physics.friction = 0.00;
    timer::start(missile->turnTimer, 300);

    localState.duoMissiles.push_back(std::unique_ptr<DuoMissile>(missile));
    enqueueAction(localState, new PlaySound("missile_spawn"), 0);
  }

public:
  static void setNextDuoMissileTimer(State& state) {
    int ms = 10000 + rand() % 10000;
    timer::start(state.duoMissileSpawnTimer, ms);
  }
  static void setNextDuoMissileSpawnBushTimer(DuoMissile& missile) {
    int ms = 500 + rand() % 2000;
    timer::start(missile.spawnBushTimer, ms);
  }
  SpawnDuoMissile() {}
};

} // namespace actions

} // namespace program