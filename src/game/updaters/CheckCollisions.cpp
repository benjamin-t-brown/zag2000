#include "CheckCollisions.h"
#include "game/State.h"
#include "game/actions/collisions/DoCollisionBulletBomber.hpp"
#include "game/actions/collisions/DoCollisionBulletBush.hpp"
#include "game/actions/collisions/DoCollisionBulletTrain.hpp"
#include "game/actions/collisions/DoCollisionPlayerBomber.hpp"
#include "game/actions/collisions/DoCollisionPlayerCollisionCircle.hpp"
#include "game/actions/collisions/DoCollisionPlayerTrain.hpp"

namespace program {

constexpr int PLAYER_COLLIDE_WIDTH = 18;
constexpr int PLAYER_COLLIDE_HEIGHT = 25;

bool collidesWithRect(double x,
                      double y,
                      double w,
                      double h,
                      double rectX,
                      double rectY,
                      double rectW,
                      double rectH) {
  return (x >= rectX - rectW / 2. && x <= rectX + rectW / 2. &&
          y >= rectY - rectH / 2. && y <= rectY + rectH / 2.);
}

bool collidesRectCircle(double rectX,
                        double rectY,
                        double rectW,
                        double rectH,
                        double circleX,
                        double circleY,
                        double radius) {
  double closestX =
      std::max(rectX - rectW / 2., std::min(circleX, rectX + rectW / 2.));
  double closestY =
      std::max(rectY - rectH / 2., std::min(circleY, rectY + rectH / 2.));
  double distanceX = circleX - closestX;
  double distanceY = circleY - closestY;

  return (distanceX * distanceX + distanceY * distanceY) < (radius * radius);
}

void checkCollisionBulletBush(State& state, Bullet& bullet, Bush& bush) {
  // if (bullet.physics.x >= bush.x - TILE_WIDTH_D / 2. &&
  //     bullet.physics.x <= bush.x + TILE_WIDTH_D / 2. &&
  //     bullet.physics.y >= bush.y - TILE_HEIGHT_D / 2. &&
  //     bullet.physics.y <= bush.y + TILE_HEIGHT_D / 2.) {
  //   enqueueAction(state, new actions::DoCollisionBulletBush(&bullet, &bush),
  //   0);
  // }
  if (collidesWithRect(bullet.physics.x,
                       bullet.physics.y,
                       bullet.w,
                       bullet.h,
                       bush.x,
                       bush.y,
                       TILE_WIDTH_D,
                       TILE_HEIGHT_D)) {
    enqueueAction(state, new actions::DoCollisionBulletBush(&bullet, &bush), 0);
  }
}

void checkCollisionBulletTrain(State& state, Bullet& bullet, Train& train) {
  if (collidesWithRect(bullet.physics.x,
                       bullet.physics.y,
                       bullet.w,
                       bullet.h,
                       train.x,
                       train.y,
                       train.w,
                       train.h)) {
    enqueueAction(
        state, new actions::DoCollisionBulletTrain(&bullet, &train), 0);
  }
}

void checkCollisionPlayerTrain(State& state, Player& player, Train& train) {
  if (collidesWithRect(player.physics.x,
                       player.physics.y,
                       PLAYER_COLLIDE_WIDTH,
                       PLAYER_COLLIDE_HEIGHT,
                       train.x,
                       train.y,
                       train.w,
                       train.h)) {
    enqueueAction(state, new actions::DoCollisionPlayerTrain(&train), 0);
  }
}

void checkCollisionPlayerBomber(State& state, Player& player, Bomber& bomber) {
  if (collidesWithRect(player.physics.x,
                       player.physics.y,
                       PLAYER_COLLIDE_WIDTH,
                       PLAYER_COLLIDE_HEIGHT,
                       bomber.physics.x,
                       bomber.physics.y,
                       TILE_WIDTH_D,
                       TILE_HEIGHT_D)) {
    enqueueAction(state, new actions::DoCollisionPlayerBomber(&bomber), 0);
  }
}

void checkCollisionPlayerCollisionCircle(State& state,
                                         Player& player,
                                         CollisionCircle& circle) {
  if (collidesRectCircle(player.physics.x,
                         player.physics.y,
                         PLAYER_COLLIDE_WIDTH,
                         PLAYER_COLLIDE_HEIGHT,
                         circle.x,
                         circle.y,
                         circle.radius)) {
    enqueueAction(state, new actions::DoCollisionPlayerCollisionCircle(), 0);
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

    for (auto& bomber : state.bombers) {
      if (bomber->shouldRemove) {
        continue;
      }
      if (collidesWithRect(bullet->physics.x,
                           bullet->physics.y,
                           bullet->w,
                           bullet->h,
                           bomber->physics.x,
                           bomber->physics.y,
                           TILE_WIDTH_D,
                           TILE_HEIGHT_D)) {
        enqueueAction(
            state,
            new actions::DoCollisionBulletBomber(bullet.get(), bomber.get()),
            0);
      }
    }
  }

  for (auto& trainHead : state.trainHeads) {
    auto& train = *trainHead;
    if (train.shouldRemove) {
      continue;
    }
    checkCollisionPlayerTrain(state, state.player, train);
    Train* nextTrain = train.next.get();
    while (nextTrain) {
      checkCollisionPlayerTrain(state, state.player, *nextTrain);
      nextTrain = nextTrain->next.get();
      if (state.player.dead) {
        return;
      }
    }
  }

  for (auto& bomber : state.bombers) {
    checkCollisionPlayerBomber(state, state.player, *bomber);
    if (state.player.dead) {
      return;
    }
  }
  for (auto& circle : state.collisionCircles) {
    checkCollisionPlayerCollisionCircle(state, state.player, *circle);
    if (state.player.dead) {
      return;
    }
  }
}
} // namespace program