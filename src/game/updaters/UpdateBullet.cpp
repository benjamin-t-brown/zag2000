#include "UpdateBullet.h"

namespace program {
void updateBullet(Bullet& bullet, State& state, int dt) {
  physics::updatePhysics(bullet.physics, dt);

  if (bullet.physics.y < state.playAreaYOffset) {
    bullet.shouldRemove = true;
  }
}
} // namespace program