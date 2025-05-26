#pragma once

#include "game/State.h"
#include "lib/sdl2w/Events.h"

namespace program {
void updatePlayer(Player& player, State& state, sdl2w::Events& events, int dt);
} // namespace program