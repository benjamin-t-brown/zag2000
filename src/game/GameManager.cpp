#include "GameManager.h"
#include "State.h"
#include "actions/spawns/SpawnBush.hpp"
#include "actions/spawns/SpawnLevelBushes.hpp"
#include "actions/spawns/SpawnTrain.hpp"
#include "lib/sdl2w/Window.h"
#include "updaters/UpdateTrain.h"
#include "utils/Timer.hpp"

namespace program {
GameManager::GameManager(sdl2w::Window& windowA)
    : window(windowA), r(windowA) {}

GameManager::~GameManager() {}

void GameManager::load() { r.setup(); }

void GameManager::start() {
  auto [renderWidth, renderHeight] = window.getDraw().getRenderSize();
  state.playAreaWidthTiles = 15;
  state.playAreaHeightTiles = 17;
  state.playAreaXOffset =
      renderWidth / 2 - state.playAreaWidthTiles * TILE_WIDTH / 2;
  state.playAreaYOffset =
      renderHeight / 2 - state.playAreaHeightTiles * TILE_HEIGHT / 2;

  enqueueAction(state, new actions::SpawnBush(std::make_pair(15 - 4, 1)), 0);
  enqueueAction(state, new actions::SpawnBush(std::make_pair(15 - 2, 2)), 0);
  enqueueAction(state, new actions::SpawnLevelBushes(), 0);
  enqueueAction(
      state, new actions::SpawnTrain(std::make_pair(9, 0), 5, 0.15), 0);
}

void GameManager::handleKeyPress(const std::string& key) {}

void GameManager::update(int dt) {

  for (auto& trainHead : state.trainHeads) {
    updateTrain(*trainHead, state, dt);
  }

  moveSequentialActions(state);
  while (!state.sequentialActions.empty()) {
    auto& delayedActionPtr = state.sequentialActions.front();
    actions::AsyncAction& delayedAction = *delayedActionPtr;
    if (delayedAction.action.get() != nullptr) {
      delayedAction.action->execute(&state);
      delayedAction.action = nullptr;
    }

    timer::update(delayedAction.timer, dt);
    if (timer::isComplete(delayedAction.timer)) {
      bool shouldLoop = delayedAction.timer.duration == 0;
      state.sequentialActions.erase(state.sequentialActions.begin());
      if (shouldLoop) {
        moveSequentialActions(state);
        continue;
      } else {
        break;
      }
    } else {
      break;
    }
  }
  for (unsigned int i = 0; i < state.parallelActions.size(); i++) {
    auto& delayedActionPtr = state.parallelActions[i];
    actions::AsyncAction& delayedAction = *delayedActionPtr;
    timer::update(delayedAction.timer, dt);
    if (timer::isComplete(delayedAction.timer)) {
      if (delayedAction.action != nullptr) {
        delayedAction.action->execute(&state);
      }
      state.parallelActions.erase(state.parallelActions.begin() + i);
      i--;
    }
  }
}

void GameManager::render() {

  window.getDraw().drawRect(state.playAreaXOffset,
                            state.playAreaYOffset,
                            state.playAreaWidthTiles * TILE_WIDTH,
                            state.playAreaHeightTiles * TILE_HEIGHT,
                            {57, 57, 57, 255});

  for (const auto& pair : state.bushes) {
    r.renderBush(*pair.second);
  }

  for (const auto& trainHead : state.trainHeads) {
    r.renderTrainFromHead(*trainHead);
  }

  for (const auto& soundName : state.soundsToPlay) {
    window.playSound(soundName);
  }
  state.soundsToPlay.clear();
}

} // namespace program