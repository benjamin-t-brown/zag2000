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

void Render::setup(State& state) {
  animations.clear();
  statePtr = &state;
}

sdl2w::Animation& Render::getAnim(const std::string& name) {
  if (animations.find(name) == animations.end()) {
    sdl2w::AnimationDefinition def =
        window.getStore().getAnimationDefinition(name);
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

int Render::getSpriteOffsetLevel(int offset) {
  return (getState().level + offset) % 4;
}

void Render::renderBush(const Bush& bush) {
  if (bush.hp <= 0) {
    return;
  }

  int bushInd = 4 * bush.variant + 4 - bush.hp;

  std::stringstream ss;
  ss << "bush" << getSpriteOffsetLevel(bush.marked ? 1 : 0) << "_" << bushInd;
  window.getDraw().drawSprite(
      //
      window.getStore().getSprite(ss.str()),
      RenderableParams{//
                       .scale = {TILE_SCALE, TILE_SCALE},
                       .x = bush.x,
                       .y = bush.y});
}

void Render::renderPlayer(const Player& player) {
  if (player.dead) {
    return;
  }
  std::stringstream ss;
  int spriteInd = 0;
  ss << "player_";

  if (player.controls.left) {
    spriteInd = 2;
  } else if (player.controls.right) {
    spriteInd = 4;
  }

  if (player.canShoot) {
    spriteInd++;
  }
  ss << spriteInd;

  window.getDraw().drawSprite(
      //
      window.getStore().getSprite(ss.str()),
      RenderableParams{//
                       .scale = {TILE_SCALE, TILE_SCALE},
                       .x = static_cast<int>(player.physics.x),
                       .y = static_cast<int>(player.physics.y)});
}

void Render::renderTrain(const Train& train) {
  std::stringstream ss;
  ss << (train.isHead ? "train" : "cart");
  ss << getSpriteOffsetLevel();
  bool flip = false;

  if (train.isRotating) {
    double pct = timer::getPct(train.rotationTimer);
    if (pct < 0.25) {
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
    } else if (pct < 0.50) {
      // down/up
      if (train.vDirection == TRAIN_UP) {
        ss << "_up";
      } else {
        ss << "_down";
      }

    } else if (pct < 0.75) {
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
    } else {
      ss << "_left";
      if (train.hDirection == TRAIN_RIGHT) {
        flip = true;
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

void Render::renderTrainFromHead(const Train& head) {
  if (head.next) {
    renderTrainFromHead(*head.next.get());
  }
  renderTrain(head);
}

void Render::renderBullet(const Bullet& bullet) {
  window.getDraw().drawRect(static_cast<int>(bullet.physics.x) - bullet.w / 2,
                            static_cast<int>(bullet.physics.y) - bullet.h / 2,
                            bullet.w,
                            bullet.h,
                            {255, 255, 255, 255});
}

void Render::renderBomber(const Bomber& bomber) {}

void Render::renderAirplane(const Airplane& plane) {}

void Render::renderDuoMissile(const DuoMissile& missile) {}

void Render::renderParticle(const Particle& particle) {
  if (particle.animation->isInitialized()) {
    window.getDraw().drawAnimation(
        //
        *particle.animation,
        RenderableParamsEx{
            //
            .scale = {TILE_SCALE, TILE_SCALE},
            .x = particle.x,
            .y = particle.y,
        });
  }
}

} // namespace program