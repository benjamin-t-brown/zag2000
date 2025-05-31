#include "UpdatePlayer.h"
#include "client/Keys.hpp"
#include "game/State.h"
#include "game/actions/player/PlayerShoot.hpp"
#include "lib/sdl2w/Events.h"
#include "utils/Timer.hpp"

namespace program {
void setControlState(State& state, sdl2w::Events& events) {
  if (events.isKeyPressed(getConfirmKey()) || events.isKeyPressed("Space")) {
    state.player.controls.shoot = true;
  } else {
    state.player.controls.shoot = false;
  }

  if (events.isKeyPressed(getUpKey())) {
    state.player.controls.up = true;
    state.player.controls.down = false;
  } else if (events.isKeyPressed(getDownKey())) {
    state.player.controls.down = true;
    state.player.controls.up = false;
  } else {
    state.player.controls.up = false;
    state.player.controls.down = false;
  }

  if (events.isKeyPressed(getLeftKey())) {
    state.player.controls.left = true;
    state.player.controls.right = false;
  } else if (events.isKeyPressed(getRightKey())) {
    state.player.controls.right = true;
    state.player.controls.left = false;
  } else {
    state.player.controls.left = false;
    state.player.controls.right = false;
  }
}

void updatePlayer(Player& player, State& state, sdl2w::Events& events, int dt) {
  const double ACCELERATION = player.acc;

  setControlState(state, events);

  if (player.controls.up) {
    physics::applyForce(player.physics, 0., ACCELERATION);
  } else if (player.controls.down) {
    physics::applyForce(player.physics, 180., ACCELERATION);
  }
  if (player.controls.left) {
    physics::applyForce(player.physics, 270., ACCELERATION);
  } else if (player.controls.right) {
    physics::applyForce(player.physics, 90., ACCELERATION);
  }

  std::vector<physics::Rect> obstacles;
  for (const auto& bush : state.bushes) {
    if (!bush.second->shouldRemove) {
      obstacles.push_back(physics::Rect{
          static_cast<double>(bush.second->x) - TILE_WIDTH_D / 2.,
          static_cast<double>(bush.second->y) - TILE_HEIGHT_D / 2.,
          TILE_WIDTH_D,
          TILE_HEIGHT_D});
    }
  }

  physics::updatePhysics(player.physics, dt, obstacles);

  const double tw = TILE_WIDTH;
  const double th = TILE_HEIGHT;

  // Check for collisions with the play area boundaries
  if (player.physics.x < state.playAreaXOffset + tw / 2) {
    player.physics.x = state.playAreaXOffset + tw / 2;
  } else if (player.physics.x > state.playAreaXOffset +
                                    state.playAreaWidthTiles * TILE_WIDTH -
                                    tw / 2) {
    player.physics.x =
        state.playAreaXOffset + state.playAreaWidthTiles * TILE_WIDTH - tw / 2;
  }

  if (player.physics.y < state.playAreaBottomYStart + th / 2) {
    player.physics.y = state.playAreaBottomYStart + th / 2;
  } else if (player.physics.y > state.playAreaYOffset +
                                    state.playAreaHeightTiles * TILE_HEIGHT -
                                    th / 2) {
    player.physics.y = state.playAreaYOffset +
                       state.playAreaHeightTiles * TILE_HEIGHT - th / 2;
  }

  timer::update(player.shootTimer, dt);

  if (player.canShoot) {
    if (player.controls.shoot) {
      enqueueAction(state, new actions::PlayerShoot(), 0);
    }
  } else {
    if (state.bullets.size() == 0 && timer::isComplete(player.shootTimer)) {
      // update methods should be able to own their own state
      player.canShoot = true;
    }
  }
}
} // namespace program