#include "UpdateBomber.h"
#include "game/State.h"
#include "game/actions/spawns/SpawnBomb.hpp"
#include "game/actions/spawns/SpawnBomber.hpp"
#include "game/actions/spawns/SpawnCollisionCircle.hpp"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "utils/Timer.hpp"
#include <cmath>

namespace program {
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
    timer::start(bomber.shootTimer, 1000);
  }
}

void updateBomb(Bomb& bomb, State& state, int dt) {
  timer::update(bomb.transformTimer, dt);
  if (timer::isComplete(bomb.transformTimer)) {
    // boom
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
} // namespace program