#include "GameManager.h"
#include "State.h"
#include "actions/control/SetControlState.hpp"
#include "actions/level/StartGame.hpp"
#include "actions/level/StartMenu.hpp"
#include "client/Keys.hpp"
#include "lib/sdl2w/EmscriptenHelpers.h"
#include "lib/sdl2w/Window.h"
#include "updaters/CheckCollisions.h"
#include "updaters/CheckGameFlow.h"
#include "updaters/UpdateEntities.h"
#include "updaters/UpdatePlayer.h"
#include "updaters/UpdateTrain.h"
#include "utils/Timer.hpp"

namespace program {
GameManager::GameManager(sdl2w::Window& windowA)
    : window(windowA), r(windowA) {}

GameManager::~GameManager() {}

void GameManager::load() {
  r.setup(state);
  auto [renderWidth, renderHeight] = window.getDraw().getRenderSize();
  state.playAreaWidthTiles = renderWidth / TILE_WIDTH;
  state.playAreaHeightTiles = renderHeight / TILE_HEIGHT - 1 + 1;
  state.playAreaXOffset =
      renderWidth / 2 - state.playAreaWidthTiles * TILE_WIDTH / 2;
  state.playAreaYOffset = 0;
  state.playAreaBottomYStart =
      (state.playAreaHeightTiles - 5) * TILE_HEIGHT + state.playAreaYOffset;
  emshelpers::notifyGameReady();
}

void GameManager::start() {
  state.controlState = CONTROL_MENU;
  //
  actions::StartMenu startMenuAction;
  startMenuAction.execute(&state);
}

void GameManager::handleKeyPress(const std::string& key) {
  // LOG(INFO) << "Key pressed: " << key << LOG_ENDL;
  if (state.controlState == CONTROL_MENU) {
    if (isConfirmKey(key)) {

      state.controlState = CONTROL_WAITING;
      enqueueAction(state, new actions::StartGame(), 0);
      emshelpers::notifyGameStarted();
    }
  } else if (state.controlState == CONTROL_IN_GAME) {
  } else if (state.controlState == CONTROL_SHOWING_HIGH_SCORE) {
    enqueueAction(state, new actions::SetControlState(CONTROL_MENU), 0);
  }
}

void GameManager::handleKeyRelease(const std::string& key) {}

void GameManager::update(int dt) {
  if (state.controlState == CONTROL_MENU) {
    updateMenu(state, dt);
  }
  updatePlayer(state.player, state, window.getEvents(), dt);

  if (state.controlState == CONTROL_IN_GAME ||
      state.controlState == CONTROL_MENU) {
    for (int i = 0; i < static_cast<int>(state.trainHeads.size()); i++) {
      auto& trainHead = state.trainHeads[i];
      updateTrain(*trainHead, state, dt);

      if (trainHead->shouldRemove) {
        state.trainHeads.erase(state.trainHeads.begin() + i);
        i--;
        continue;
      }

      Train* prevTrain = trainHead.get();
      Train* nextTrain = trainHead->next.get();
      while (nextTrain != nullptr) {
        if (nextTrain->shouldRemove) {
          prevTrain->next = nullptr;
          break;
        }
        prevTrain = nextTrain;
        nextTrain = nextTrain->next.get();
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

    for (int i = 0; i < static_cast<int>(state.airplanes.size()); i++) {
      auto& airplane = *state.airplanes[i];
      updateAirplane(airplane, state, dt);
      if (airplane.shouldRemove) {
        state.airplanes.erase(state.airplanes.begin() + i);
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

    for (int i = 0; i < static_cast<int>(state.duoMissiles.size()); i++) {
      auto& duoMissile = *state.duoMissiles[i];
      updateDuoMissile(duoMissile, state, dt);
      if (duoMissile.shouldRemove) {
        state.duoMissiles.erase(state.duoMissiles.begin() + i);
        i--;
        continue;
      }
    }

    checkCollisions(state);
    updateSpawners(state, dt);
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

  updateState(state, dt);
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

  for (const auto& airplane : state.airplanes) {
    r.renderAirplane(*airplane);
  }

  for (const auto& duoMissile : state.duoMissiles) {
    r.renderDuoMissile(*duoMissile);
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