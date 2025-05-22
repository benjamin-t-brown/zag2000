#pragma once

#include "game/State.h"

namespace program {
int getPlayerAreaIndFromPos(State& state, int x, int y);
int getTrainLookAheadInd(State& state, Train& head);
int getTrainInd(State& state, Train& head);
void updateTrain(Train& head, State& state, int dt);
} // namespace program