#include "CheckGameFlow.h"
#include "game/State.h"
#include "game/actions/level/EndGame.hpp"
#include "game/actions/level/RestartLevel.hpp"
#include "game/actions/level/RestartMenu.hpp"
#include "game/actions/level/TransitionToNextLevel.hpp"
#include "game/actions/ui/PlaySound.hpp"
#include "lib/sdl2w/EmscriptenHelpers.h"

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
          LOG(INFO) << "END GAME" << LOG_ENDL;
          enqueueAction(state, new actions::EndGame(), 0);
        }
      }
    } else if (state.trainHeads.size() == 0) {
      enqueueAction(state, new actions::TransitionToNextLevel(), 0);
    }
  }
  if (state.controlState == CONTROL_MENU) {
    if (state.sequentialActionsNext.empty()) {
      if (state.player.dead) {
        if (!state.controlIsWaitingForGameOver) {
          state.controlIsWaitingForGameOver = true;
          enqueueAction(state, new actions::RestartMenu(), 0);
        }
      } else if (state.trainHeads.size() == 0) {
        LOG(INFO) << "No trains left, restarting menu, achievement unlocked!"
                  << LOG_ENDL;
        enqueueAction(state, new actions::RestartMenu(), 0);
        emshelpers::notifyGameGeneric("menu_no_trains_left");
      }
    }
  }

  if (state.controlState == CONTROL_IN_GAME) {
    const int extraLifeThreshold = 10000;
    int relativeScore =
        state.player.score - state.numExtraLives * extraLifeThreshold;
    if (relativeScore >= extraLifeThreshold) {
      state.numExtraLives++;
      state.player.lives++;
      enqueueAction(state, new actions::PlaySound("extra_life"), 0);
    }
  }
}
} // namespace program