#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"

namespace program {

namespace actions {

inline Train* createTrain(State& state, int xTile, int yTile, double speed) {
  Train* train = new Train();
  train->x = xTile * TILE_WIDTH + state.playAreaXOffset + TILE_WIDTH / 2.;
  train->y = yTile * TILE_HEIGHT + state.playAreaYOffset + TILE_HEIGHT / 2.;
  train->prevY = train->y;
  train->speed = speed;
  state.trainHeads.push_back(std::unique_ptr<Train>(train));
  return train;
}

inline Train* createCart(Train* head) {
  Train* node = head;
  while (node->next) {
    node = node->next.get();
  }
  Train* cart = new Train();
  cart->x = head->x;
  cart->y = head->y;
  cart->speed = head->speed;
  cart->prevY = head->prevY;
  cart->hDirection = head->hDirection;
  cart->vDirection = head->vDirection;
  cart->isHead = false;
  node->next = std::unique_ptr<Train>(cart);
  return cart;
}

class SpawnTrain : public AbstractAction {
  std::pair<int, int> tilePos;
  int numCarts;
  double speed;
  TrainDirectionH hDirection;

  void act() override {
    State& localState = *this->state;

    auto [xTile, yTile] = tilePos;

    auto head1 = createTrain(localState, xTile, yTile, speed);
    head1->hDirection = hDirection;
    for (int i = 0; i < numCarts; i++) {
      createCart(head1);
    }
  }

public:
  static void setAdditionalTrainSpawnTimer(State& state) {
    timer::start(state.additionalTrainSpawnTimer, 14000);
  }

  SpawnTrain(std::pair<int, int> tilePos,
             int numCarts,
             double speed,
             TrainDirectionH hDirection = TRAIN_RIGHT)
      : tilePos(tilePos),
        numCarts(numCarts),
        speed(speed),
        hDirection(hDirection) {}
};

} // namespace actions

} // namespace program