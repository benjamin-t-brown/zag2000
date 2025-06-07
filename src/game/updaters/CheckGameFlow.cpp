#include "CheckGameFlow.h"
#include "game/State.h"
#include "game/actions/level/EndGame.hpp"
#include "game/actions/level/RestartLevel.hpp"
#include "game/actions/level/TransitionToNextLevel.hpp"

namespace program {
void checkGameFlow(State& state, int dt) {
  if (state.controlState == CONTROL_IN_GAME) {
    if (state.player.dead) {
      if (!state.controlIsWaitingForGameOver) {
        state.controlIsWaitingForGameOver = true;
        if (state.player.lives > 0) {
          enqueueAction(state, new actions::RestartLevel(), 0);
          state.player.lives--;
        } else {
          enqueueAction(state, new actions::EndGame(), 0);
        }
      }
    } else if (state.trainHeads.size() == 0) {
      enqueueAction(state, new actions::TransitionToNextLevel(), 0);
    }
  }
}
} // namespace program