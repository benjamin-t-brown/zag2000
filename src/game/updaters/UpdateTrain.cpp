#include "UpdateTrain.h"
#include <cmath>

namespace program {

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
  const double upperBound = state.playAreaBottomYStart;

  if (head.isPoisoned) {
    head.isRotating = true;
    timer::start(head.rotationTimer);
    trainSwapHDirection(head);
    head.prevX = head.x;
  } else if (nextX > rightBound && head.hDirection == TRAIN_RIGHT) {
    head.isRotating = true;
    timer::start(head.rotationTimer);
    trainSwapHDirection(head);
    head.prevX = rightBound;
  } else if (nextX < leftBound && head.hDirection == TRAIN_LEFT) {
    head.isRotating = true;
    timer::start(head.rotationTimer);
    trainSwapHDirection(head);
    head.prevX = leftBound;
  } else {
    double prevX = head.x;
    head.x = nextX;

    int trainAheadInd = getTrainLookAheadInd(state, head);
    if (isBushAtInd(state, trainAheadInd)) {
      head.isRotating = true;
      head.x = prevX;
      timer::start(head.rotationTimer);
      trainSwapHDirection(head);
      head.prevX = prevX;
    } else if (isAnotherTrainAtInd(state, head, trainAheadInd)) {
      head.isRotating = true;
      head.x = prevX;
      timer::start(head.rotationTimer);
      trainSwapHDirection(head);
      head.prevX = prevX;
    }
  }

  // rotating set this tick
  if (head.isRotating) {
    if (head.vDirection == TRAIN_DOWN && head.y + head.h > lowerBound) {
      head.vDirection = TRAIN_UP;
    } else if (head.vDirection == TRAIN_UP && head.y - head.h < upperBound) {
      head.vDirection = TRAIN_DOWN;
    }
  }
}

void updateTrain(Train& head, State& state, int dt) {
  double dx = head.hDirection == TRAIN_RIGHT ? head.speed : -head.speed;
  double nextX = head.x + dx * dt;

  if (!head.isRotating) {
    trainCheckSetHeadRotationOrNext(head, state, nextX);
  }
  // dont else this
  if (head.isRotating) {
    // weird swap
    double dx = head.hDirection == TRAIN_LEFT ? head.speed : -head.speed;
    double nextX = head.x + dx * dt;
    if (head.hDirection == TRAIN_LEFT) {
      double diffDist = nextX - head.x;
      double distOverage =
          head.rotationTimer.t + diffDist - head.rotationTimer.duration;
      timer::update(head.rotationTimer, diffDist);
      if (timer::isComplete(head.rotationTimer)) {
        head.isRotating = false;
        timer::start(head.rotationTimer);
        head.x = head.prevX - distOverage;
        head.y = head.vDirection == TRAIN_DOWN ? head.prevY + head.h
                                               : head.prevY - head.h;
        head.prevY = head.y;

        if (head.isPoisoned) {
          const double lowerBound =
              state.playAreaYOffset + state.playAreaHeightTiles * TILE_HEIGHT;
          const double upperBound = state.playAreaBottomYStart;
          if (head.vDirection == TRAIN_DOWN && head.y + head.h > lowerBound) {
            head.isPoisoned = false;
          } else if (head.vDirection == TRAIN_UP &&
                     head.y - head.h < upperBound) {
            head.isPoisoned = false;
          }
        }
      } else {
        head.isRotating = true;
        double t = timer::getPct(head.rotationTimer);
        double offset = (head.w / 4.) * std::sin(PI * t) *
                        (head.hDirection == TRAIN_LEFT ? 1 : -1);
        head.x = head.prevX + offset;
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
        head.isRotating = false;
        timer::start(head.rotationTimer);
        head.x = head.prevX + diffDist + distOverage;
        head.y =
            head.prevY + (head.vDirection == TRAIN_DOWN ? head.h : -head.h);
        head.prevY = head.y;

        if (head.isPoisoned) {
          const double lowerBound =
              state.playAreaYOffset + state.playAreaHeightTiles * TILE_HEIGHT;
          const double upperBound = state.playAreaBottomYStart;
          if (head.vDirection == TRAIN_DOWN && head.y + head.h > lowerBound) {
            head.isPoisoned = false;
          } else if (head.vDirection == TRAIN_UP &&
                     head.y - head.h < upperBound) {
            head.isPoisoned = false;
          }
        }
      } else {
        head.isRotating = true;
        double t = timer::getPct(head.rotationTimer);
        double offset = (head.w / 4.) * std::sin(PI * t) *
                        (head.hDirection == TRAIN_LEFT ? 1 : -1);
        head.x = head.prevX + offset;
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
        cart->prevX = cart->x;
      }
    }

    if (cart->isRotating) {
      cart->rotationTimer.t += dt * cart->speed;

      double distOverage = cart->rotationTimer.t - cart->rotationTimer.duration;
      if (timer::isComplete(cart->rotationTimer)) {
        cart->isRotating = false;
        timer::start(cart->rotationTimer);
        nextX = (prev->hDirection == TRAIN_LEFT ? prev->prevX - distOverage
                                                : prev->prevX + distOverage);
        cart->y = (cart->vDirection == TRAIN_DOWN ? cart->prevY + cart->h
                                                  : cart->prevY - cart->h);
        cart->prevY = cart->y;
        const double lowerBound =
            state.playAreaYOffset + state.playAreaHeightTiles * TILE_HEIGHT;
        const double upperBound = state.playAreaBottomYStart;
        if (cart->vDirection == TRAIN_DOWN && cart->y + cart->h > lowerBound) {
          cart->vDirection = TRAIN_UP;
        } else if (cart->vDirection == TRAIN_UP &&
                   cart->y - cart->h < upperBound) {
          cart->vDirection = TRAIN_DOWN;
        }
      } else {
        nextX = prev->hDirection == TRAIN_LEFT ? cart->prevX : cart->prevX;
        double t = timer::getPct(cart->rotationTimer);
        double offset = (cart->w / 4.) * std::sin(PI * t) *
                        (cart->hDirection == TRAIN_LEFT ? 1 : -1);
        nextX = nextX + offset;
        cart->y = cart->prevY + (cart->vDirection == TRAIN_DOWN
                                     ? static_cast<double>(cart->h) *
                                           timer::getPct(cart->rotationTimer)
                                     : -static_cast<double>(cart->h) *
                                           timer::getPct(cart->rotationTimer));
      }
    }

    cart->x = nextX;

    prev = cart;
    cart = cart->next.get();
  }
}
} // namespace program