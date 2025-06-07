
#pragma once

#include "game/State.h"
namespace program {
void updateBomber(Bomber& bomber, State& state, int dt);
void updateBomb(Bomb& bomb, State& state, int dt);
} // namespace program