#include "Render.h"
#include "game/State.h"
#include "lib/sdl2w/Animation.h"
#include "lib/sdl2w/Draw.h"
#include "lib/sdl2w/L10n.h"
#include "lib/sdl2w/Window.h"
#include "utils/Timer.hpp"
#include <cmath>
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

void Render::renderBg() {
  const State& state = *statePtr;
  sdl2w::Draw& d = window.getDraw();
  auto [renderW, _] = d.getRenderSize();

  for (int i = 0; i < state.playAreaWidthTiles; i++) {
    for (int j = 0; j < state.playAreaHeightTiles; j++) {
      int bgInd = state.bg[j * state.playAreaWidthTiles + i];
      std::stringstream ss;
      ss << "bg" << getSpriteOffsetLevel() << "_" << bgInd;
      d.drawSprite(
          //
          window.getStore().getSprite(ss.str()),
          RenderableParams{
              //
              .scale = {TILE_SCALE, TILE_SCALE},
              .x = state.playAreaXOffset + i * TILE_WIDTH + TILE_WIDTH / 2,
              .y = state.playAreaYOffset + j * TILE_HEIGHT + TILE_WIDTH / 2});
    }
  }

  d.drawSprite(
      window.getStore().getSprite("bg_lower_gradient_0"),
      RenderableParams{
          .scale = {state.playAreaWidthTiles + 1, 5},
          .x = state.playAreaXOffset + renderW / 2 - TILE_WIDTH / 2,
          .y = state.playAreaYOffset + state.playAreaHeightTiles * TILE_HEIGHT,
      });
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

void Render::renderBomber(const Bomber& bomber) {
  Animation& anim = getAnim("bomber_anim");
  window.getDraw().drawAnimation(
      //
      anim,
      RenderableParamsEx{
          //
          .scale = {TILE_SCALE, TILE_SCALE},
          .x = static_cast<int>(bomber.physics.x),
          .y = static_cast<int>(bomber.physics.y),
          .flipped = false,
      });
}

void Render::renderBomb(const Bomb& bomb) {
  double pct = timer::getPct(bomb.transformTimer);
  int x = pct * (bomb.x2 - bomb.x) + bomb.x;
  int y = pct * (bomb.y2 - bomb.y) + bomb.y;

  y -= (TILE_HEIGHT * 2) * std::sin(timer::getPct(bomb.transformTimer) * M_PI);

  {
    Animation& anim = getAnim("bomb_anim");
    window.getDraw().drawAnimation(
        //
        anim,
        RenderableParamsEx{
            //
            .scale = {TILE_SCALE, TILE_SCALE},
            .x = x,
            .y = y,
            .flipped = false,
        });
  }

  {
    Animation& anim = getAnim("bomb_target");
    window.getDraw().drawAnimation(
        //
        anim,
        RenderableParamsEx{
            //
            .scale = {TILE_SCALE, TILE_SCALE},
            .x = static_cast<int>(bomb.x2),
            .y = static_cast<int>(bomb.y2),
            .flipped = false,
        });
  }
}

void Render::renderAirplane(const Airplane& plane) {
  Animation& anim = getAnim("plane_left");
  window.getDraw().drawAnimation(
      //
      anim,
      RenderableParamsEx{
          //
          .scale = {TILE_SCALE, TILE_SCALE},
          .x = static_cast<int>(plane.x),
          .y = static_cast<int>(plane.y),
          .flipped = plane.vx > 0,
      });
}

void Render::renderDuoMissile(const DuoMissile& missile) {}

void Render::renderParticle(const Particle& particle) {
  if (particle.animation != nullptr && particle.animation->isInitialized()) {
    window.getDraw().drawAnimation(
        //
        *particle.animation,
        RenderableParamsEx{
            //
            .scale = {TILE_SCALE, TILE_SCALE},
            .x = particle.x,
            .y = particle.y,
        });
  } else if (!particle.text.empty()) {
    window.getDraw().drawText(particle.text,
                              sdl2w::RenderTextParams{
                                  .fontName = "default",
                                  .fontSize = sdl2w::TextSize::TEXT_SIZE_20,
                                  .x = particle.x,
                                  .y = particle.y,
                                  .color = {255, 255, 255, 255},
                                  .centered = true,
                              });
  }
}

void Render::renderCollisionCircle(const CollisionCircle& circle) {
  sdl2w::Draw& d = window.getDraw();
  d.drawCircle(static_cast<int>(circle.x),
               static_cast<int>(circle.y),
               circle.radius,
               {255, 0, 0, 100},
               false);
}

void Render::renderUi() {
  const State& state = *statePtr;
  sdl2w::Draw& d = window.getDraw();
  auto [renderW, renderH] = d.getRenderSize();

  if (state.controlState == CONTROL_MENU) {
    d.drawText(TRANSLATE("Press button to start"),
               sdl2w::RenderTextParams{
                   .fontName = "default",
                   .fontSize = sdl2w::TextSize::TEXT_SIZE_20,
                   .x = renderW / 2,
                   .y = renderH / 2,
                   .color = {255, 255, 255, 255},
                   .centered = true,
               });
    return;
  } else if (state.controlState == CONTROL_SHOWING_HIGH_SCORE) {
    d.drawRect(0, 0, renderW, TILE_HEIGHT, {77, 57, 57, 255});
    d.drawText(TRANSLATE("High Score") + ": " +
                   std::to_string(state.player.score),
               sdl2w::RenderTextParams{
                   .fontName = "default",
                   .fontSize = sdl2w::TextSize::TEXT_SIZE_24,
                   .x = renderW / 2,
                   .y = renderH / 2,
                   .color = {255, 255, 255, 255},
                   .centered = true,
               });
    return;
  }

  d.drawRect(0, 0, renderW, TILE_HEIGHT, {77, 57, 57, 255});

  d.drawText(TRANSLATE("Lives") + ": " + std::to_string(state.player.lives),
             sdl2w::RenderTextParams{
                 .fontName = "default",
                 .fontSize = sdl2w::TextSize::TEXT_SIZE_20,
                 .x = 2,
                 .y = 0,
                 .color = {255, 255, 255, 255},
                 .centered = false,
             });

  d.drawText(TRANSLATE("Score") + ": " + std::to_string(state.player.score),
             sdl2w::RenderTextParams{
                 .fontName = "default",
                 .fontSize = sdl2w::TextSize::TEXT_SIZE_20,
                 .x = renderW / 2,
                 .y = 12,
                 .color = {255, 255, 255, 255},
                 .centered = true,
             });

  d.drawText(TRANSLATE("Level") + ": " + std::to_string(state.level),
             sdl2w::RenderTextParams{
                 .fontName = "default",
                 .fontSize = sdl2w::TextSize::TEXT_SIZE_20,
                 .x = renderW - 100,
                 .y = 0,
                 .color = {255, 255, 255, 255},
                 .centered = false,
             });

  d.drawText(std::to_string(state.trainHeads.size()) + " " +
                 TRANSLATE("Trains"),
             sdl2w::RenderTextParams{
                 .fontName = "default",
                 .fontSize = sdl2w::TextSize::TEXT_SIZE_20,
                 .x = renderW - 100,
                 .y = 100,
                 .color = {255, 255, 255, 255},
                 .centered = false,
             });
  // render position of train
  int ctr = 1;
  for (const auto& trainHead : state.trainHeads) {
    d.drawText(std::to_string(static_cast<int>(trainHead->x)) + ", " +
                   std::to_string(static_cast<int>(trainHead->y)),
               sdl2w::RenderTextParams{
                   .fontName = "default",
                   .fontSize = sdl2w::TextSize::TEXT_SIZE_20,
                   .x = renderW - 100,
                   .y = 100 + 20 * (ctr),
                   .color = {255, 255, 255, 255},
                   .centered = true,
               });
    ctr++;
  }
}

} // namespace program