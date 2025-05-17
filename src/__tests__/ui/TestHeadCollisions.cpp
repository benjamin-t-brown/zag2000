// make testui TestHeadCollisions

#include "../setupTest.hpp"
#include "client/Render.h"
#include "game/State.h"
#include "lib/sdl2w/Logger.h"
#include "utils/Timer.hpp"
#include <SDL2/SDL_timer.h>
#include <memory>

using namespace program;

Train* createTrain(int x, int y) {
  Train* train = new Train();
  train->x = x;
  train->y = y;
  train->prevY = y;
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

void updateTrain(Train& head, int dt) {
  double dx = head.hDirection == TRAIN_RIGHT ? head.speed : -head.speed;
  double nextX = head.x + dx * dt;
  const double halfWidth = head.w / 2.;


  const double leftBound = halfWidth;
  const double rightBound = 100. - halfWidth;
  const double w = 100.;

  if (nextX > w - halfWidth && head.hDirection == TRAIN_RIGHT &&
      !head.isRotating) {
    head.isRotating = true;
    timer::start(head.rotationTimer);
    head.hDirection = head.hDirection == TRAIN_RIGHT ? TRAIN_LEFT : TRAIN_RIGHT;

    // LOG(INFO) << "x: " << head.x << " diff" << diffDist << " "
    //           << timer::getPct(head.rotationTimer) << LOG_ENDL;
  } else if (nextX < 0 + halfWidth && head.hDirection == TRAIN_LEFT &&
             !head.isRotating) {
    // head.hDirection = head.hDirection == TRAIN_RIGHT ? TRAIN_LEFT :
    // TRAIN_RIGHT;

    head.isRotating = true;
    timer::start(head.rotationTimer);
    head.hDirection = head.hDirection == TRAIN_RIGHT ? TRAIN_LEFT : TRAIN_RIGHT;

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
  } else if (!head.isRotating) {
    head.x = nextX;
  }

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
        LOG(INFO) << "FLIP" << LOG_ENDL;
        head.isRotating = false;
        timer::start(head.rotationTimer);
        head.x = w - halfWidth - distOverage;
        head.y = head.prevY + head.h;
        head.prevY = head.y;
      } else {
        head.isRotating = true;
        head.x = w - halfWidth;
        head.y = head.prevY + static_cast<double>(head.h) *
                                  timer::getPct(head.rotationTimer);
      }
    } else {
      double diffDist = head.x - nextX;
      double distOverage =
          head.rotationTimer.t + diffDist - head.rotationTimer.duration;
      timer::update(head.rotationTimer, diffDist);
      if (timer::isComplete(head.rotationTimer)) {
        LOG(INFO) << "FLIP" << LOG_ENDL;
        head.isRotating = false;
        timer::start(head.rotationTimer);
        head.x = halfWidth + diffDist + distOverage;
        head.y = head.prevY + head.h;
        head.prevY = head.y;
      } else {
        head.isRotating = true;
        head.x = halfWidth;
        head.y = head.prevY + static_cast<double>(head.h) *
                                  timer::getPct(head.rotationTimer);
      }
    }
  }

  Train* prev = &head;
  Train* cart = head.next.get();
  while (cart) {
    double nextX =
        prev->x + (cart->hDirection == TRAIN_RIGHT ? -head.w : head.w);

    if (prev->hDirection == cart->hDirection && !prev->isRotating) {
      // keep it
      cart->y = prev->y;
    } else if (!cart->isRotating && prev->isRotating) {
      nextX += (cart->hDirection == TRAIN_RIGHT ? prev->rotationTimer.t
                                                : -prev->rotationTimer.t);
    }
    // begin rotation when directions are different and this cart is not
    // rotating begin rotation when directions are the same and this cart is not
    // rotating
    else if (!cart->isRotating &&
             ((prev->hDirection != cart->hDirection) ||
              (prev->hDirection == cart->hDirection && prev->isRotating))) {
      cart->isRotating = true;
      timer::start(cart->rotationTimer);
      cart->hDirection =
          cart->hDirection == TRAIN_RIGHT ? TRAIN_LEFT : TRAIN_RIGHT;

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

    if (cart->isRotating) {
      // assumes grid-based rotation (could break with collisions to other
      // trains unless that's also grid based)
      if (prev->isRotating) {
        LOG(INFO) << "Determine from prev rotation" << LOG_ENDL;
        cart->rotationTimer.t = prev->rotationTimer.t;
      } else {
        double prevDiffFromWall =
        (prev->hDirection == TRAIN_LEFT ? w - halfWidth - prev->x
          : prev->x - 0 - halfWidth);
          // double remainingDist = prev->w - prevDiffFromWall;
          // LOG(INFO) << "Determine from prev dist: " << prevDiffFromWall << " " << cart->rotationTimer.duration << LOG_ENDL;
        cart->rotationTimer.t = prevDiffFromWall;
      }

      double distOverage = cart->rotationTimer.t - cart->rotationTimer.duration;
      if (timer::isComplete(cart->rotationTimer)) {
        cart->isRotating = false;
        timer::start(prev->rotationTimer);
        nextX = (prev->hDirection == TRAIN_LEFT ? w - halfWidth - distOverage
                                                : halfWidth + distOverage);
        cart->y = cart->prevY + cart->h;
        cart->prevY = cart->y;
        LOG(INFO) << "FLIP Cart" << LOG_ENDL;
      } else {
        nextX = prev->hDirection == TRAIN_LEFT ? w - halfWidth : halfWidth;
        cart->y = cart->prevY + static_cast<double>(cart->h) *
                                    timer::getPct(cart->rotationTimer);
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

  auto& events = window.getEvents();
  auto& d = window.getDraw();
  // events.setKeyboardEvent(
  //     sdl2w::ON_KEY_PRESS,
  //     [&](const std::string& key, int) { game.handleKeyPress(key); });

  Render gameRenderer(window);
  gameRenderer.setup();

  auto head1 = std::unique_ptr<Train>(createTrain(50, 10));
  // createCart(head1.get());
  // createCart(head1.get());

  auto _initializeLoop = [&]() {
    sdl2w::renderSplash(window);
    return true;
  };

  auto _onInitialized = [&]() {

  };

  auto _mainLoop = [&]() {
    int dt = window.getDeltaTime();
    d.setBackgroundColor({10, 10, 10});

    gameRenderer.updateAnimations(dt);

    updateTrain(*head1, dt);
    window.getDraw().drawRect(0, 0, 100, 100, {127, 127, 127, 255});
    renderTrainFromHead(*head1, gameRenderer);

    return true;
  };

  window.startRenderLoop(_initializeLoop, _onInitialized, _mainLoop);

  delete windowPtr;
  delete storePtr;

  sdl2w::Window::unInit();
  LOG(INFO) << "End program" << LOG_ENDL;
}