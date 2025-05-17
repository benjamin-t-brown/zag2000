#include "Render.h"
#include "lib/sdl2w/Animation.h"
#include "lib/sdl2w/Draw.h"
#include "lib/sdl2w/Window.h"
#include <memory>
#include <sstream>

namespace program {

using sdl2w::Animation;
using sdl2w::RenderableParams;
using sdl2w::RenderableParamsEx;

constexpr double TILE_SCALE = 1;

Render::Render(sdl2w::Window& windowA) : window(windowA) {}

void Render::setup() { animations.clear(); }

sdl2w::Animation& Render::getAnim(const std::string& name) {
  if (animations.find(name) == animations.end()) {
    sdl2w::AnimationDefinition def =
        window.getStore().getAnimationDefinition(name);
    LOG(INFO) << "Creating animation: " << name << LOG_ENDL;
    animations[name] =
        std::unique_ptr<Animation>(new Animation(def, window.getStore()));
  }
  return *animations[name];
}

void Render::updateAnimations(int dt) {
  for (auto& anim : animations) {
    anim.second->update(dt);
  }
}

void Render::renderBush(const Bush& bush) {
  if (bush.hp <= 0) {
    return;
  }

  std::stringstream ss;
  ss << "bush0" << 4 - bush.hp;
  window.getDraw().drawSprite(
      //
      window.getStore().getSprite(ss.str()),
      RenderableParams{//
                       .scale = {1., 1.},
                       .x = bush.x,
                       .y = bush.y});
}

void Render::renderPlayer(const Player& player) {
  if (player.dead) {
    return;
  }
  std::stringstream ss;
  ss << "player" << (player.canShoot ? 1 : 0);
  window.getDraw().drawSprite(
      //
      window.getStore().getSprite(ss.str()),
      RenderableParams{//
                       .scale = {1., 1.},
                       .x = static_cast<int>(player.physics.x),
                       .y = static_cast<int>(player.physics.y)});
}

void Render::renderTrain(const Train& train) {
  std::stringstream ss;
  ss << (train.isHead ? "train0" : "cart0");
  bool flip = false;

  if (train.isRotating) {
    double pct = timer::getPct(train.rotationTimer);
    if (pct < 0.33) {
      // leftdown/rightdown OR leftup/rightup
      if (train.vDirection == TRAIN_UP) {
        if (train.hDirection == TRAIN_LEFT) {
          ss << "_leftup";
          flip = true;
        } else {
          ss << "_leftup";
        }
      } else {
        if (train.hDirection == TRAIN_LEFT) {
          ss << "_leftdown";
          flip = true;
        } else {
          ss << "_leftdown";
        }
      }
    } else if (pct < 0.66) {
      // down/up
      if (train.vDirection == TRAIN_UP) {
        ss << "_up";
      } else {
        ss << "_down";
      }

    } else {
      // rightdown/leftdown or rightup/leftup
      if (train.vDirection == TRAIN_UP) {
        if (train.hDirection == TRAIN_LEFT) {
          ss << "_leftup";
        } else {
          ss << "_leftup";
          flip = true;
        }
      } else {
        if (train.hDirection == TRAIN_LEFT) {
          ss << "_leftdown";
        } else {
          ss << "_leftdown";
          flip = true;
        }
      }
    }
  } else {
    ss << "_left";
    if (train.hDirection == TRAIN_RIGHT) {
      flip = true;
    }
  }

  Animation& anim = getAnim(ss.str());
  window.getDraw().drawAnimation(
      //
      anim,
      RenderableParamsEx{
          //
          .scale = {TILE_SCALE, TILE_SCALE},
          .x = static_cast<int>(train.x),
          .y = static_cast<int>(train.y),
          .flipped = flip,
      });
}

void Render::renderBullet(const Bullet& bullet) {}

void Render::renderBomber(const Bomber& bomber) {}

void Render::renderAirplane(const Airplane& plane) {}

void Render::renderDuoMissile(const DuoMissile& missile) {}

void Render::renderParticle(const Particle& particle) {}

} // namespace program