#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "lib/hiscore/hiscore.h"

namespace program {

namespace actions {

class SetControlState : public AbstractAction {
  ControlState controlState;
  void act() override {
    State& localState = *this->state;

    localState.controlState = controlState;
    localState.controlIsWaitingForGameOver = false;
    if (controlState == CONTROL_IN_GAME || controlState == CONTROL_MENU) {
      localState.player.dead = false;
    }
    if (controlState == CONTROL_SHOWING_HIGH_SCORE) {
      localState.lastScore = localState.player.score;
      if (localState.player.score > localState.highScore) {
        localState.highScore = localState.player.score;
        localState.soundsToPlay.push_back("zag_hiscore");
      }
      localState.player.score = 0;
      hiscore::saveHighScores({{"player", localState.highScore}});
    }
  }

public:
  SetControlState(ControlState controlState) : controlState(controlState) {}
};

} // namespace actions

} // namespace program