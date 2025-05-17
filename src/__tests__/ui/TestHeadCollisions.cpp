// make testui TestHeadCollisions

#include "../setupTest.hpp"
#include "client/Render.h"
#include "game/State.h"
#include "lib/sdl2w/Logger.h"
#include "utils/Timer.hpp"
#include <SDL2/SDL_timer.h>
#include <memory>

using namespace program;

Train* createTrain(State& state, int xTile, int yTile) {
  Train* train = new Train();
  train->x = xTile * TILE_WIDTH + state.playAreaXOffset + TILE_WIDTH / 2.;
  train->y = yTile * TILE_HEIGHT + state.playAreaYOffset + TILE_HEIGHT / 2.;
  train->prevY = train->y;
  state.trainHeads.push_back(std::unique_ptr<Train>(train));
  return train;
}

Train* createCart(Train* head) {
  Train* node = head;
  while (node->next) {
    node = node->next.get();
  }
  Train* cart = new Train();
  cart->prevY = head->prevY;
  cart->hDirection = head->hDirection;
  cart->vDirection = head->vDirection;
  cart->isHead = false;
  node->next = std::unique_ptr<Train>(cart);
  return cart;
}

Bush* createBush(State& state, int xTile, int yTile) {
  int bushInd = xTile + yTile * state.playAreaWidthTiles;
  // if bush already exists, skip
  if (state.bushes.find(bushInd) != state.bushes.end()) {
    return nullptr;
  }

  Bush* bush = new Bush();
  bush->x = xTile * TILE_WIDTH + state.playAreaXOffset + TILE_WIDTH / 2.;
  bush->y = yTile * TILE_HEIGHT + state.playAreaYOffset + TILE_HEIGHT / 2.;
  state.bushes[bushInd] = std::unique_ptr<Bush>(bush);
  return bush;
}

int getPlayerAreaIndFromPos(State& state, int x, int y) {
  int xTile = (x - state.playAreaXOffset) / TILE_WIDTH;
  int yTile = (y - state.playAreaYOffset) / TILE_HEIGHT;
  if (xTile < 0 || xTile >= state.playAreaWidthTiles || yTile < 0 ||
      yTile >= state.playAreaHeightTiles) {
    return -1;
  }
  return xTile + yTile * state.playAreaWidthTiles;
}

int getTrainLookAheadInd(State& state, Train& head) {
  int headOffset = head.hDirection == TRAIN_RIGHT ? head.w / 2 : -head.w / 2;
  return getPlayerAreaIndFromPos(state, head.x + headOffset, head.y);
}

int getTrainInd(State& state, Train& head) {
  return getPlayerAreaIndFromPos(state, head.x, head.y);
}

bool isBushAtInd(State& state, int ind) {
  return state.bushes.find(ind) != state.bushes.end();
}

bool isAnotherTrainAtInd(State& state, Train& myTrain, int ind) {
  for (const auto& train : state.trainHeads) {
    if (train.get() == &myTrain) {
      continue;
    }
    if (getTrainInd(state, *train) == ind) {
      return true;
    }
    Train* cart = train->next.get();
    while (cart) {
      if (getTrainInd(state, *cart) == ind) {
        return true;
      }
      cart = cart->next.get();
    }
  }
  return false;
}

void trainSwapHDirection(Train& train) {
  train.hDirection = train.hDirection == TRAIN_RIGHT ? TRAIN_LEFT : TRAIN_RIGHT;
}

void trainCheckSetHeadRotationOrNext(Train& head, State& state, double nextX) {
  const double halfWidth = head.w / 2.;
  const double leftBound = state.playAreaXOffset + halfWidth;
  const double rightBound =
      state.playAreaXOffset + state.playAreaWidthTiles * TILE_WIDTH - halfWidth;
  const double lowerBound =
      state.playAreaYOffset + state.playAreaHeightTiles * TILE_HEIGHT;
  const double upperBound = state.playAreaYOffset;

  if (nextX > rightBound && head.hDirection == TRAIN_RIGHT) {
    LOG(INFO) << "START HEAD ROTATION" << LOG_ENDL;
    head.isRotating = true;
    timer::start(head.rotationTimer);
    trainSwapHDirection(head);
    head.prevX = rightBound;

    // LOG(INFO) << "x: " << head.x << " diff" << diffDist << " "
    //           << timer::getPct(head.rotationTimer) << LOG_ENDL;
  } else if (nextX < leftBound && head.hDirection == TRAIN_LEFT) {
    LOG(INFO) << "START HEAD ROTATION" << LOG_ENDL;
    // head.hDirection = head.hDirection == TRAIN_RIGHT ? TRAIN_LEFT :
    // TRAIN_RIGHT;

    head.isRotating = true;
    timer::start(head.rotationTimer);
    trainSwapHDirection(head);
    head.prevX = leftBound;

    // timer::update(head.rotationTimer, diffDist);
    // if (timer::isComplete(head.rotationTimer)) {
    //   head.isRotating = false;
    //   timer::start(head.rotationTimer, 0);
    //   head.x = 0 + halfWidth + diffDist;
    // } else {
    //   head.isRotating = true;
    //   head.x = halfWidth;
    // }
    // head.x = w - halfWidth - diffDist;

    // LOG(INFO) << "x: " << head.x << " diff" << diffDist << LOG_ENDL;
  } else {
    double prevX = head.x;
    head.x = nextX;

    int trainAheadInd = getTrainLookAheadInd(state, head);
    // int trainInd = getTrainInd(state, head);
    if (isBushAtInd(state, trainAheadInd)) {
      LOG(INFO) << "START HEAD ROTATION" << LOG_ENDL;
      head.isRotating = true;
      head.x = prevX;
      timer::start(head.rotationTimer);
      trainSwapHDirection(head);
      head.prevX = prevX;
    } else if (isAnotherTrainAtInd(state, head, trainAheadInd)) {
      LOG(INFO) << "START HEAD ROTATION BONK" << LOG_ENDL;
      head.isRotating = true;
      head.x = prevX;
      timer::start(head.rotationTimer);
      trainSwapHDirection(head);
      head.prevX = prevX;
    }
  }

  // rotating set this tick
  if (head.isRotating) {
    if (head.y + head.h > lowerBound) {
      head.vDirection = TRAIN_UP;
    } else if (head.y - head.h < upperBound) {
      head.vDirection = TRAIN_DOWN;
    }
  }
}

void updateTrain(Train& head, State& state, int dt) {
  double dx = head.hDirection == TRAIN_RIGHT ? head.speed : -head.speed;
  double nextX = head.x + dx * dt;
  const double lowerBound =
      state.playAreaYOffset + state.playAreaHeightTiles * TILE_HEIGHT;
  const double upperBound = state.playAreaYOffset;
  // const double halfWidth = head.w / 2.;
  // const double leftBound = state.playAreaXOffset + halfWidth;
  // const double rightBound =
  //     state.playAreaXOffset + state.playAreaWidthTiles * TILE_WIDTH -
  //     halfWidth;
  // const double w = 100.;

  if (!head.isRotating) {
    trainCheckSetHeadRotationOrNext(head, state, nextX);
    // if (nextX > rightBound && head.hDirection == TRAIN_RIGHT) {
    //   head.isRotating = true;
    //   timer::start(head.rotationTimer);
    //   trainSwapHDirection(head);
    //   head.prevX = rightBound;

    //   // LOG(INFO) << "x: " << head.x << " diff" << diffDist << " "
    //   //           << timer::getPct(head.rotationTimer) << LOG_ENDL;
    // } else if (nextX < leftBound && head.hDirection == TRAIN_LEFT) {
    //   // head.hDirection = head.hDirection == TRAIN_RIGHT ? TRAIN_LEFT :
    //   // TRAIN_RIGHT;

    //   head.isRotating = true;
    //   timer::start(head.rotationTimer);
    //   trainSwapHDirection(head);
    //   head.prevX = leftBound;

    //   // timer::update(head.rotationTimer, diffDist);
    //   // if (timer::isComplete(head.rotationTimer)) {
    //   //   head.isRotating = false;
    //   //   timer::start(head.rotationTimer, 0);
    //   //   head.x = 0 + halfWidth + diffDist;
    //   // } else {
    //   //   head.isRotating = true;
    //   //   head.x = halfWidth;
    //   // }
    //   // head.x = w - halfWidth - diffDist;

    //   // LOG(INFO) << "x: " << head.x << " diff" << diffDist << LOG_ENDL;
    // } else {
    //   double prevX = head.x;
    //   head.x = nextX;

    //   int trainInd = getTrainInd(state, head);
    //   if (isBushAtInd(state, trainInd)) {
    //     head.isRotating = true;
    //     head.x = prevX;
    //     timer::start(head.rotationTimer);
    //     trainSwapHDirection(head);
    //     head.prevX = prevX;
    //   }
    // }
  }
  // dont else this
  if (head.isRotating) {
    // weird swap
    double dx = head.hDirection == TRAIN_LEFT ? head.speed : -head.speed;
    double nextX = head.x + dx * dt;
    if (head.hDirection == TRAIN_LEFT) {
      double diffDist = nextX - head.x;
      // double distRemaining = head.rotationTimer.duration -
      // head.rotationTimer.t;
      double distOverage =
          head.rotationTimer.t + diffDist - head.rotationTimer.duration;
      timer::update(head.rotationTimer, diffDist);
      if (timer::isComplete(head.rotationTimer)) {
        LOG(INFO) << "Head done rotating" << LOG_ENDL;
        head.isRotating = false;
        timer::start(head.rotationTimer);
        head.x = head.prevX - distOverage;
        // if (head.vDirection
        head.y = head.vDirection == TRAIN_DOWN ? head.prevY + head.h
                                               : head.prevY - head.h;
        head.prevY = head.y;
        // if (head.y + head.h
        // trainCheckSetHeadRotationOrNext(head, state, head.x);
      } else {
        head.isRotating = true;
        head.x = head.prevX;
        // head.y = head.prevY + static_cast<double>(head.h) *
        //                           timer::getPct(head.rotationTimer);
        head.y = head.prevY + (head.vDirection == TRAIN_DOWN
                                   ? static_cast<double>(head.h) *
                                         timer::getPct(head.rotationTimer)
                                   : -static_cast<double>(head.h) *
                                         timer::getPct(head.rotationTimer));
      }
    } else {
      double diffDist = head.x - nextX;
      double distOverage =
          head.rotationTimer.t + diffDist - head.rotationTimer.duration;
      timer::update(head.rotationTimer, diffDist);
      if (timer::isComplete(head.rotationTimer)) {
        LOG(INFO) << "Head done rotating" << LOG_ENDL;
        head.isRotating = false;
        timer::start(head.rotationTimer);
        head.x = head.prevX + diffDist + distOverage;
        head.y =
            head.prevY + (head.vDirection == TRAIN_DOWN ? head.h : -head.h);
        head.prevY = head.y;
        // trainCheckSetHeadRotationOrNext(head, state, head.x);
      } else {
        head.isRotating = true;
        head.x = head.prevX;
        head.y = head.prevY + (head.vDirection == TRAIN_DOWN
                                   ? static_cast<double>(head.h) *
                                         timer::getPct(head.rotationTimer)
                                   : -static_cast<double>(head.h) *
                                         timer::getPct(head.rotationTimer));
      }
    }
  }

  Train* prev = &head;
  Train* cart = head.next.get();
  while (cart) {
    double nextX =
        prev->x + (cart->hDirection == TRAIN_RIGHT ? -head.w : head.w);

    if (!cart->isRotating) {
      // update position
      if (prev->hDirection == cart->hDirection) {
        // keep it
        cart->y = prev->y;
      } else if (prev->isRotating) {
        nextX += (cart->hDirection == TRAIN_RIGHT ? prev->rotationTimer.t
                                                  : -prev->rotationTimer.t);
      }

      // check if cart should be rotating
      if (((prev->hDirection != cart->hDirection && !prev->isRotating) ||
           (prev->hDirection == cart->hDirection && prev->isRotating))) {
        cart->isRotating = true;
        timer::start(cart->rotationTimer);
        trainSwapHDirection(*cart);
        cart->vDirection = prev->vDirection;
        cart->prevX = cart->x;
        // cart->prevY = cart->y;
        LOG(INFO) << "  Start cart rotation" << LOG_ENDL;
        // cart->prevX =
        // cart->hDirection =
        //     cart->hDirection == TRAIN_RIGHT ? TRAIN_LEFT : TRAIN_RIGHT;

        // distance away from wall
        // double diffFromWall =
        //     (prev->hDirection == TRAIN_LEFT ? w - halfWidth - prev->x
        //                                     : prev->x - 0 - halfWidth);
        // double remainingDist = prev->w - diffFromWall;
        // if (remainingDist > 0) {
        //   nextX = prev->hDirection == TRAIN_LEFT ? w - halfWidth -
        //   remainingDist
        //                                          : remainingDist + halfWidth;
        // } else {
        //   cart->hDirection =
        //       cart->hDirection == TRAIN_RIGHT ? TRAIN_LEFT : TRAIN_RIGHT;
        //   nextX = prev->x + (cart->hDirection == TRAIN_RIGHT ? -head.w :
        //   head.w);
        // }
      }
    }

    if (cart->isRotating) {
      // assumes grid-based rotation (could break with collisions to other
      // trains unless that's also grid based)
      if (prev->isRotating) {
        // LOG(INFO) << "Determine from prev rotation" << LOG_ENDL;
        cart->rotationTimer.t = prev->rotationTimer.t;
      } else {
        double prevDiffFromWall =
            (prev->hDirection == TRAIN_LEFT ? prev->prevX - prev->x
                                            : prev->x - prev->prevX);
        // double remainingDist = prev->w - prevDiffFromWall;
        // LOG(INFO) << "Determine from prev dist: " << prevDiffFromWall << " "
        // << cart->rotationTimer.duration << LOG_ENDL;
        // if (cart->rotationTimer.t
        // if (prevDiffFromWall > cart->rotationTimer.t) {
        //   cart->rotationTimer.t = prevDiffFromWall;
        // } else {
        //   // rough approx
        // }
        cart->rotationTimer.t += dt * cart->speed;
      }

      double distOverage = cart->rotationTimer.t - cart->rotationTimer.duration;
      if (timer::isComplete(cart->rotationTimer)) {
        cart->isRotating = false;
        timer::start(prev->rotationTimer);
        nextX = (prev->hDirection == TRAIN_LEFT ? prev->prevX - distOverage
                                                : prev->prevX + distOverage);
        // cart->y = cart->prevY + cart->h;
        cart->y = (cart->vDirection == TRAIN_DOWN ? cart->prevY + cart->h
                                                  : cart->prevY - cart->h);
        cart->prevY = cart->y;
        LOG(INFO) << "  Cart done rotating" << LOG_ENDL;
      } else {
        nextX = prev->hDirection == TRAIN_LEFT ? prev->prevX : prev->prevX;
        // cart->y = cart->prevY + static_cast<double>(cart->h) *
        //                             timer::getPct(cart->rotationTimer);
        cart->y = cart->prevY + (cart->vDirection == TRAIN_DOWN
                                     ? static_cast<double>(cart->h) *
                                           timer::getPct(cart->rotationTimer)
                                     : -static_cast<double>(cart->h) *
                                           timer::getPct(cart->rotationTimer));
      }
      // if (remainingDist > 0) {
      //   nextX = prev->hDirection == TRAIN_LEFT ? w - halfWidth -
      //   remainingDist
      //                                          : remainingDist + halfWidth;
      // } else {
      //   cart->hDirection =
      //       cart->hDirection == TRAIN_RIGHT ? TRAIN_LEFT : TRAIN_RIGHT;
      //   nextX = prev->x + (cart->hDirection == TRAIN_RIGHT ? -head.w :
      //   head.w);
      // }

      // double distRemaining = head.rotationTimer.duration -
      // head.rotationTimer.t;
      // double distOverage =
      //     head.rotationTimer.t + diffDist - head.rotationTimer.duration;
      // timer::update(head.rotationTimer, diffDist);
      // if (timer::isComplete(head.rotationTimer)) {
      //   LOG(INFO) << "FLIP" << LOG_ENDL;
      //   head.isRotating = false;
      //   timer::start(head.rotationTimer);
      //   head.x = w - halfWidth - distOverage;
      //   head.y = head.prevY + head.h;
      //   head.prevY = head.y;
      // } else {
      //   head.isRotating = true;
      //   head.x = w - halfWidth;
      //   head.y = head.prevY + static_cast<double>(head.h) *
      //                             timer::getPct(head.rotationTimer);
      // }
    }

    cart->x = nextX;

    prev = cart;
    cart = cart->next.get();
  }

  // if (dt > 0) {
  //   // HACK prevent flicker BUT AT WHAT COST
  //   updateTrain(head, state, 0);
  // }
}

void renderTrainFromHead(Train& head, Render& renderer) {
  if (head.next) {
    renderTrainFromHead(*head.next.get(), renderer);
  }
  renderer.renderTrain(head);
}

int main(int argc, char** argv) {
  const int w = 640;
  const int h = 480;

  LOG(INFO) << "Start program" << LOG_ENDL;
  auto [windowPtr, storePtr] = test::setupTest(argc, argv, w, h);
  sdl2w::Window& window = *windowPtr;
  State state;

  state.playAreaHeightTiles = 10;
  state.playAreaWidthTiles = 10;
  state.playAreaXOffset = 50;
  state.playAreaYOffset = 100;

  auto& events = window.getEvents();
  auto& d = window.getDraw();
  // events.setKeyboardEvent(
  //     sdl2w::ON_KEY_PRESS,
  //     [&](const std::string& key, int) { game.handleKeyPress(key); });

  Render gameRenderer(window);
  gameRenderer.setup();

  // auto head1 = createTrain(state, 3, 0);
  // for (int i = 0; i < 5; i++) {
  //   createCart(head1);
  // }
  // createCart(head1);
  // createCart(head1);
  // createCart(head1);
  // createCart(head1);

  // auto head2 = createTrain(state, 8, 1);
  // head2->hDirection = TRAIN_LEFT;
  // for (int i = 0; i < 5; i++) {
  //   createCart(head2);
  // }

  auto head3 = createTrain(state, 5, 9);
  for (int i = 0; i < 5; i++) {
    createCart(head3);
  }

  createBush(state, 5, 0);
  createBush(state, 6, 2);
  createBush(state, 3, 1);

  // createBush(state, 5, 0);
  // createBush(state, 3, 1);
  // createBush(state, 5, 2);
  // createBush(state, 3, 3);
  // createBush(state, 5, 4);
  // createBush(state, 3, 5);
  // createBush(state, 5, 6);
  // createBush(state, 3, 7);

  auto _initializeLoop = [&]() {
    sdl2w::renderSplash(window);
    return true;
  };

  auto _onInitialized = [&]() {

  };

  int timeDilation = 0;

  auto _mainLoop = [&]() {
    int dt = window.getDeltaTime();
    d.setBackgroundColor({10, 10, 10});

    gameRenderer.updateAnimations(dt - timeDilation);

    for (const auto& trainHead : state.trainHeads) {
      updateTrain(*trainHead, state, dt - timeDilation);
    }

    window.getDraw().drawRect(state.playAreaXOffset,
                              state.playAreaYOffset,
                              state.playAreaWidthTiles * TILE_WIDTH,
                              state.playAreaHeightTiles * TILE_HEIGHT,
                              {127, 127, 127, 255});

    for (const auto& pair : state.bushes) {
      gameRenderer.renderBush(*pair.second);
    }

    for (const auto& trainHead : state.trainHeads) {
      renderTrainFromHead(*trainHead, gameRenderer);
    }

    // LOG(INFO) << "IND: " << getPlayerAreaIndFromPos(state, head1->x,
    // head1->y)
    //           << LOG_ENDL;

    return true;
  };

  window.startRenderLoop(_initializeLoop, _onInitialized, _mainLoop);

  delete windowPtr;
  delete storePtr;

  sdl2w::Window::unInit();
  LOG(INFO) << "End program" << LOG_ENDL;
}