#include "UpdateEntities.h"
#include "game/State.h"
#include "game/actions/spawns/SpawnAirplane.hpp"
#include "game/actions/spawns/SpawnBomb.hpp"
#include "game/actions/spawns/SpawnBomber.hpp"
#include "game/actions/spawns/SpawnBush.hpp"
#include "game/actions/spawns/SpawnCollisionCircle.hpp"
#include "game/actions/spawns/SpawnDuoMissile.hpp"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/actions/spawns/SpawnTrain.hpp"
#include "utils/Timer.hpp"
#include <cmath>

namespace program {
void updateBullet(Bullet& bullet, State& state, int dt) {
  physics::updatePhysics(bullet.physics, dt);

  if (bullet.physics.y < state.playAreaYOffset) {
    bullet.shouldRemove = true;
  }
}

void updateBomber(Bomber& bomber, State& state, int dt) {
  double nextAngle = physics::getAngleTowards(
      bomber.physics,
      std::make_pair<double, double>(bomber.walkX, bomber.walkY));
  double currAngle = bomber.heading.angle;
  if (currAngle <= nextAngle) {
    if (std::abs(currAngle - nextAngle) < 180) {
      bomber.heading.turnDirection = HeadingTurnDirection::RIGHT;
    } else {
      bomber.heading.turnDirection = HeadingTurnDirection::LEFT;
    }
  } else {
    if (std::abs(currAngle - nextAngle) < 180) {
      bomber.heading.turnDirection = HeadingTurnDirection::LEFT;
    } else {
      bomber.heading.turnDirection = HeadingTurnDirection::RIGHT;
    }
  }
  physics::applyForce(bomber.physics, bomber.heading.angle, bomber.speed);
  physics::updatePhysics(bomber.physics, dt);
  physics::updateHeading(bomber.heading, dt);

  double distanceToWalk =
      std::sqrt(std::pow(bomber.walkX - bomber.physics.x, 2) +
                std::pow(bomber.walkY - bomber.physics.y, 2));
  if (distanceToWalk < 25.0) {
    actions::SpawnBomber::setNewWalkTarget(bomber, state);
  }

  timer::update(bomber.shootTimer, dt);
  if (timer::isComplete(bomber.shootTimer)) {
    double targetX = bomber.physics.x + rand() % 100 - 50;
    double targetY = bomber.physics.y + rand() % 100 - 50;
    auto bomberPos = physics::getPos(bomber.physics);
    std::pair<double, double> target = std::make_pair(targetX, targetY);
    enqueueAction(state, new actions::SpawnBomb(bomberPos, target), 0);
    timer::start(bomber.shootTimer);
  }
}

void updateBomb(Bomb& bomb, State& state, int dt) {
  timer::update(bomb.transformTimer, dt);
  if (timer::isComplete(bomb.transformTimer)) {
    enqueueAction(state,
                  new actions::SpawnParticle(
                      actions::PARTICLE_BOMB_EXPL, bomb.x2, bomb.y2, 400),
                  0);
    enqueueAction(
        state,
        new actions::SpawnCollisionCircle(std::make_pair(bomb.x2, bomb.y2)),
        0);
    bomb.shouldRemove = true;
  }
}

void updateAirplane(Airplane& airplane, State& state, int dt) {
  airplane.x += airplane.vx * dt;
  if (airplane.x < state.playAreaXOffset ||
      airplane.x >
          state.playAreaXOffset + state.playAreaWidthTiles * TILE_WIDTH) {
    airplane.shouldRemove = true;
  }
  timer::update(airplane.engineSoundTimer, dt);
  if (timer::isComplete(airplane.engineSoundTimer)) {
    state.soundsToPlay.push_back("airplane");
    timer::start(airplane.engineSoundTimer);
  }
}

void updateDuoMissile(DuoMissile& missile, State& state, int dt) {
  physics::updatePhysics(missile.physics, dt);
  if (missile.type == DUO_MISSILE_SINGLE) {
    if (missile.physics.vy == 0) {
      timer::update(missile.turnTimer, dt);
      if (timer::isComplete(missile.turnTimer)) {
        missile.physics.vy = std::abs(missile.physics.vx) + 0.03;
        missile.physics.vx = 0;
      }
    }
  } else {
    timer::update(missile.spawnBushTimer, dt);
    if (timer::isComplete(missile.spawnBushTimer)) {
      actions::SpawnDuoMissile::setNextDuoMissileSpawnBushTimer(missile);
      int tileX = static_cast<int>((missile.physics.x - state.playAreaXOffset) /
                                   TILE_WIDTH);
      int tileY = static_cast<int>((missile.physics.y - state.playAreaYOffset) /
                                   TILE_HEIGHT);
      if (tileX < 0 || tileX >= state.playAreaWidthTiles || tileY < 0 ||
          tileY >= state.playAreaHeightTiles) {
        return;
      }
      enqueueAction(
          state, new actions::SpawnBush(std::make_pair(tileX, tileY)), 0);
    }
  }
}

void updateSpawners(State& state, int dt) {
  timer::update(state.bomberSpawnTimer, dt);
  if (timer::isComplete(state.bomberSpawnTimer)) {
    actions::SpawnBomber::setNextBomberTimer(state);
    int numBombers = state.bombers.size();
    if ((state.level <= 3 && numBombers < 1) ||
        (state.level > 3 && numBombers < 3)) {
      enqueueAction(state, new actions::SpawnBomber(rand() % 2), 0);
    }
  }

  timer::update(state.additionalTrainSpawnTimer, dt);
  if (timer::isComplete(state.additionalTrainSpawnTimer)) {
    actions::SpawnTrain::setAdditionalTrainSpawnTimer(state);
    TrainDirectionH hDirection = rand() % 2 == 0 ? TRAIN_RIGHT : TRAIN_LEFT;
    int xTile = hDirection == TRAIN_RIGHT ? 0 : state.playAreaWidthTiles;
    int yTile = (state.playAreaHeightTiles - 6);
    enqueueAction(
        state,
        new actions::SpawnTrain(std::make_pair(xTile, yTile),
                                0,
                                0.15 + static_cast<double>(state.level) * 0.03,
                                hDirection),
        0);
  }

  timer::update(state.airplaneSpawnTimer, dt);
  if (timer::isComplete(state.airplaneSpawnTimer)) {
    actions::SpawnAirplane::setNextAirplaneTimer(state);
    enqueueAction(
        state,
        new actions::SpawnAirplane((rand() % 2) ? TRAIN_LEFT : TRAIN_RIGHT),
        0);
  }

  timer::update(state.duoMissileSpawnTimer, dt);
  if (timer::isComplete(state.duoMissileSpawnTimer)) {
    actions::SpawnDuoMissile::setNextDuoMissileTimer(state);
    enqueueAction(state, new actions::SpawnDuoMissile(), 0);
  }
}
} // namespace program