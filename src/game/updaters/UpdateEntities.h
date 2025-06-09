#pragma once

#include "game/State.h"
namespace program {
void updateBullet(Bullet& bullet, State& state, int dt);
void updateBomber(Bomber& bomber, State& state, int dt);
void updateBomb(Bomb& bomb, State& state, int dt);
void updateAirplane(Airplane& airplane, State& state, int dt);
void updateSpawners(State& state, int dt);
} // namespace program