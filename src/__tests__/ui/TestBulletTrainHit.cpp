// make testui TestBulletTrainHit

#include "../setupTest.hpp"
#include "client/Render.h"
#include "game/State.h"
#include "game/actions/spawns/SpawnTrain.hpp"
#include "game/updaters/CheckCollisions.h"
#include "game/updaters/UpdateEntities.h"
#include "game/updaters/UpdatePlayer.h"
#include "game/updaters/UpdateTrain.h"
#include "lib/sdl2w/Animation.h"
#include "lib/sdl2w/Logger.h"
#include "utils/Timer.hpp"
#include <SDL2/SDL_timer.h>
#include <cmath>
#include <memory>

using namespace program;

int main(int argc, char** argv) {
  const int w = 640;
  const int h = 480;

  LOG(INFO) << "Start program" << LOG_ENDL;
  auto [windowPtr, storePtr] = test::setupTest(argc, argv, w, h);
  sdl2w::Window& window = *windowPtr;
  Render r(window);
  State state;
  r.setup(state);

  auto [renderW, renderH] = window.getDraw().getRenderSize();
  state.playAreaWidthTiles = renderW / TILE_WIDTH;
  state.playAreaHeightTiles = renderH / TILE_HEIGHT;
  state.playAreaXOffset = (renderW - state.playAreaWidthTiles * TILE_WIDTH) / 2;
  state.playAreaYOffset =
      (renderH - state.playAreaHeightTiles * TILE_HEIGHT) / 2;
  state.controlState = CONTROL_IN_GAME;

  state.player.physics.x =
      state.playAreaXOffset + state.playAreaWidthTiles * TILE_WIDTH / 2.;
  state.player.physics.y = state.playAreaYOffset +
                           state.playAreaHeightTiles * TILE_HEIGHT / 2. + 200;

  auto& events = window.getEvents();
  events.setKeyboardEvent(sdl2w::ON_KEY_DOWN, [&](const std::string& key, int) {
    if (key == "R") {
      actions::SpawnTrain spawnTrain(
          std::make_pair(state.playAreaHeightTiles / 2,
                         state.playAreaWidthTiles / 2),
          1,
          0.15,
          TRAIN_RIGHT);
      spawnTrain.execute(&state);
    }
  });
  events.setKeyboardEvent(sdl2w::ON_KEY_UP, [&](const std::string& key, int) {
    // game.handleKeyRelease(key);
  });

  auto _initializeLoop = [&]() {
    sdl2w::renderSplash(window);
    return true;
  };

  auto _onInitialized = [&]() {

  };

  int timeDilation = 0;

  actions::SpawnTrain spawnTrain(std::make_pair(state.playAreaHeightTiles / 2,
                                                state.playAreaWidthTiles / 2),
                                 1,
                                 0.15,
                                 TRAIN_RIGHT);
  spawnTrain.execute(&state);

  auto _mainLoop = [&]() {
    int dt = window.getDeltaTime();
    sdl2w::Draw& d = window.getDraw();
    d.setBackgroundColor({10, 10, 10});

    // update

    state.bushes.clear();
    if (state.player.dead) {
      state.player.dead = false;
    }
    updatePlayer(state.player, state, window.getEvents(), dt);

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

    for (int i = 0; i < static_cast<int>(state.bullets.size()); i++) {
      auto& bullet = state.bullets[i];
      updateBullet(*bullet, state, dt);
      if (bullet->shouldRemove) {
        state.bullets.erase(state.bullets.begin() + i);
        i--;
        continue;
      }
    }

    checkCollisions(state);
    updateState(state, dt);

    // draw

    window.getDraw().drawRect(state.playAreaXOffset,
                              state.playAreaYOffset,
                              state.playAreaWidthTiles * TILE_WIDTH,
                              state.playAreaHeightTiles * TILE_HEIGHT,
                              {127, 127, 127, 255});

    for (const auto& bullet : state.bullets) {
      r.renderBullet(*bullet);
    }

    for (const auto& trainHead : state.trainHeads) {
      r.renderTrainFromHead(*trainHead);
    }

    r.renderPlayer(state.player);

    return true;
  };

  window.startRenderLoop(_initializeLoop, _onInitialized, _mainLoop);

  delete windowPtr;
  delete storePtr;

  sdl2w::Window::unInit();
  LOG(INFO) << "End program" << LOG_ENDL;
}