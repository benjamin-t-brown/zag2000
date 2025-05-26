#include "CheckCollisions.h"
#include "game/State.h"
#include "game/actions/collisions/DoCollisionBulletBush.hpp"
#include "game/actions/collisions/DoCollisionBulletTrain.hpp"

namespace program {
void checkCollisionBulletBush(State& state, Bullet& bullet, Bush& bush) {
  if (bullet.physics.x >= bush.x - TILE_WIDTH_D / 2. &&
      bullet.physics.x <= bush.x + TILE_WIDTH_D / 2. &&
      bullet.physics.y >= bush.y - TILE_HEIGHT_D / 2. &&
      bullet.physics.y <= bush.y + TILE_HEIGHT_D / 2.) {
    enqueueAction(state, new actions::DoCollisionBulletBush(&bullet, &bush), 0);
  }
}

void checkCollisionBulletTrain(State& state, Bullet& bullet, Train& train) {
  if (bullet.physics.x >= train.x - train.w / 2. &&
      bullet.physics.x <= train.x + train.w / 2. &&
      bullet.physics.y >= train.y - train.h / 2. &&
      bullet.physics.y <= train.y + train.h / 2.) {
    enqueueAction(
        state, new actions::DoCollisionBulletTrain(&bullet, &train), 0);
  }
}

void checkCollisions(State& state) {
  for (auto& bullet : state.bullets) {
    for (auto& pair : state.bushes) {
      auto& bush = *pair.second;
      if (bush.hp <= 0) {
        continue;
      }
      checkCollisionBulletBush(state, *bullet, bush);
    }

    for (auto& trainHead : state.trainHeads) {
      auto& train = *trainHead;
      if (train.shouldRemove) {
        continue;
      }
      checkCollisionBulletTrain(state, *bullet, train);
      Train* nextTrain = train.next.get();
      while (nextTrain) {
        checkCollisionBulletTrain(state, *bullet, *nextTrain);
        nextTrain = nextTrain->next.get();
      }
    }
  }
}
} // namespace program