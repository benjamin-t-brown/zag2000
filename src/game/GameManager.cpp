#include "GameManager.h"
#include "State.h"
#include "actions/collisions/DoCollisionPlayerTrain.hpp"
#include "actions/control/SetControlState.hpp"
#include "actions/level/StartNextLevel.hpp"
#include "actions/spawns/SpawnBomber.hpp"
#include "actions/spawns/SpawnBush.hpp"
#include "actions/spawns/SpawnLevelBushes.hpp"
#include "actions/spawns/SpawnTrain.hpp"
#include "client/Keys.hpp"
#include "game/actions/spawns/SpawnTrain.hpp"
#include "lib/sdl2w/EmscriptenHelpers.h"
#include "lib/sdl2w/Window.h"
#include "updaters/CheckCollisions.h"
#include "updaters/CheckGameFlow.h"
#include "updaters/UpdateBomber.h"
#include "updaters/UpdateBullet.h"
#include "updaters/UpdatePlayer.h"
#include "updaters/UpdateTrain.h"
#include "utils/Timer.hpp"

namespace program {
GameManager::GameManager(sdl2w::Window& windowA)
    : window(windowA), r(windowA) {}

GameManager::~GameManager() {}

void GameManager::load() {
  r.setup(state);
  emshelpers::notifyGameReady();
}

void GameManager::start() {
  auto [renderWidth, renderHeight] = window.getDraw().getRenderSize();
  state.playAreaWidthTiles = renderWidth / TILE_WIDTH;
  state.playAreaHeightTiles = renderHeight / TILE_HEIGHT - 1;
  state.playAreaXOffset =
      renderWidth / 2 - state.playAreaWidthTiles * TILE_WIDTH / 2;
  state.playAreaYOffset = TILE_HEIGHT;
  // state.playAreaYOffset = renderHeight / 2 -
  //                         state.playAreaHeightTiles * TILE_HEIGHT / 2 +
  //                         TILE_HEIGHT / 2;
  state.playAreaBottomYStart =
      (state.playAreaHeightTiles - 5) * TILE_HEIGHT + state.playAreaYOffset;

  state.player.physics.x =
      state.playAreaXOffset + state.playAreaWidthTiles * TILE_WIDTH / 2.;
  state.player.physics.y =
      state.playAreaBottomYStart + TILE_HEIGHT * 2 + TILE_HEIGHT / 2.;

  for (int i = 0; i < state.playAreaWidthTiles; i++) {
    for (int j = 0; j < state.playAreaHeightTiles; j++) {
      state.bg.push_back(rand() % 10);
    }
  }

  // enqueueAction(state, new actions::SpawnBush(std::make_pair(15 - 3, 1)), 0);
  // enqueueAction(state, new actions::SpawnBush(std::make_pair(15 - 3, 2)), 0);
  // enqueueAction(state, new actions::SpawnBush(std::make_pair(15 - 3, 3)), 0);
  enqueueAction(state, new actions::StartNextLevel(1), 0);
  enqueueAction(
      state,
      new actions::SpawnTrain(std::make_pair(0, state.playAreaHeightTiles - 3),
                              5,
                              0.21,
                              TRAIN_RIGHT),
      0);
  enqueueAction(state, new actions::SpawnBomber(false), 0);

  state.controlState = CONTROL_WAITING;
  emshelpers::notifyGameStarted();
}

void GameManager::handleKeyPress(const std::string& key) {
  // LOG(INFO) << "Key pressed: " << key << LOG_ENDL;
  if (state.controlState == CONTROL_MENU) {
    if (isConfirmKey(key)) {
      start();
    }
  } else if (state.controlState == CONTROL_IN_GAME) {
  } else if (state.controlState == CONTROL_SHOWING_HIGH_SCORE) {
    enqueueAction(state, new actions::SetControlState(CONTROL_MENU), 0);
  }
  if (key == "T") {
    enqueueAction(state, new actions::DoCollisionPlayerTrain(nullptr), 0);
  }
}

void GameManager::handleKeyRelease(const std::string& key) {}

void GameManager::update(int dt) {
  updatePlayer(state.player, state, window.getEvents(), dt);

  if (state.controlState == CONTROL_IN_GAME) {
    for (int i = 0; i < static_cast<int>(state.trainHeads.size()); i++) {
      auto& trainHead = state.trainHeads[i];
      updateTrain(*trainHead, state, dt);

      Train* prevTrain = trainHead.get();
      Train* nextTrain = trainHead->next.get();
      while (nextTrain) {
        if (nextTrain->shouldRemove) {
          prevTrain->next = nullptr;
        }
        prevTrain = nextTrain;
        nextTrain = nextTrain->next.get();
      }

      if (trainHead->shouldRemove) {
        state.trainHeads.erase(state.trainHeads.begin() + i);
        i--;
        continue;
      }
    }

    for (int i = 0; i < static_cast<int>(state.bombers.size()); i++) {
      auto& bomber = *state.bombers[i];
      updateBomber(bomber, state, dt);
      if (bomber.shouldRemove) {
        state.bombers.erase(state.bombers.begin() + i);
        i--;
        continue;
      }
    }

    for (int i = 0; i < static_cast<int>(state.bombs.size()); i++) {
      auto& bomb = *state.bombs[i];
      updateBomb(bomb, state, dt);
      if (bomb.shouldRemove) {
        state.bombs.erase(state.bombs.begin() + i);
        i--;
        continue;
      }
    }

    for (int i = 0; i < static_cast<int>(state.bullets.size()); i++) {
      auto& bullet = state.bullets[i];
      updateBullet(*bullet, state, dt);
      if (bullet->shouldRemove) {
        state.bullets.erase(state.bullets.begin() + i);
        i--;
        continue;
      }
    }

    for (auto it = state.bushes.begin(); it != state.bushes.end();) {
      auto& bush = it->second;
      if (bush->shouldRemove) {
        it = state.bushes.erase(it);
      } else {
        ++it;
      }
    }

    checkCollisions(state);
    checkGameFlow(state, dt);
  }

  for (int i = 0; i < static_cast<int>(state.particles.size()); i++) {
    auto& particle = state.particles[i];
    timer::update(particle->timer, dt);
    if (timer::isComplete(particle->timer)) {
      state.particles.erase(state.particles.begin() + i);
      i--;
      continue;
    }
    if (particle->animation != nullptr &&
        !particle->animation->isInitialized()) {
      sdl2w::AnimationDefinition def =
          window.getStore().getAnimationDefinition(particle->animName);
      particle->animation = std::unique_ptr<sdl2w::Animation>(
          new sdl2w::Animation(def, window.getStore()));
    }
    if (particle->animation != nullptr) {
      particle->animation->update(dt);
    }
  }

  for (int i = 0; i < static_cast<int>(state.collisionCircles.size()); i++) {
    auto& circle = state.collisionCircles[i];
    timer::update(circle->timer, dt);
    if (timer::isComplete(circle->timer)) {
      state.collisionCircles.erase(state.collisionCircles.begin() + i);
      i--;
      continue;
    }
  }

  r.updateAnimations(dt);

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

  window.getDraw().drawRect(state.playAreaXOffset,
                            state.playAreaBottomYStart,
                            state.playAreaWidthTiles * TILE_WIDTH,
                            state.playAreaYOffset +
                                state.playAreaHeightTiles * TILE_HEIGHT -
                                state.playAreaBottomYStart,
                            {77, 77, 77, 255});

  r.renderBg();

  for (const auto& pair : state.bushes) {
    r.renderBush(*pair.second);
  }

  for (const auto& trainHead : state.trainHeads) {
    r.renderTrainFromHead(*trainHead);
  }

  for (const auto& bomber : state.bombers) {
    r.renderBomber(*bomber);
  }

  for (const auto& bomb : state.bombs) {
    r.renderBomb(*bomb);
  }

  for (const auto& bullet : state.bullets) {
    r.renderBullet(*bullet);
  }

  for (const auto& particle : state.particles) {
    r.renderParticle(*particle);
  }

  for (const auto& circle : state.collisionCircles) {
    r.renderCollisionCircle(*circle);
  }

  r.renderPlayer(state.player);

  r.renderUi();

  for (const auto& soundName : state.soundsToPlay) {
    window.playSound(soundName);
  }
  state.soundsToPlay.clear();
}

} // namespace program