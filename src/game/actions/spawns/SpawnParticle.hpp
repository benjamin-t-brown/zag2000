#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "lib/sdl2w/Animation.h"
#include <string>

namespace program {

namespace actions {

enum ParticleType {
  PARTICLE_EXPL_0,
  PARTICLE_EXPL_1,
  PARTICLE_EXPL_2,
  PARTICLE_EXPL_3,
  PARTICLE_BOMB_EXPL,
  PARTICLE_BOMB_TARGET,
  PARTICLE_TEXT
};

class SpawnParticle : public AbstractAction {
  ParticleType type;
  std::string text;
  std::pair<int, int> tilePos;
  std::pair<int, int> pos;
  int ms;
  bool calcPos = false;

  std::string particleTypeToAnimName(ParticleType type) {
    switch (type) {
    case PARTICLE_EXPL_0:
      return "expl_entity0";
    case PARTICLE_EXPL_1:
      return "expl_entity1";
    case PARTICLE_EXPL_2:
      return "expl_entity2";
    case PARTICLE_EXPL_3:
      return "expl_entity3";
    case PARTICLE_BOMB_EXPL:
      return "bomb_expl_anim";
    case PARTICLE_BOMB_TARGET:
      return "bomb_target";
    case PARTICLE_TEXT:
      return "text_particle";
    default:
      return "";
    }
  }

  void act() override {
    State& localState = *this->state;

    std::string animName = particleTypeToAnimName(type);
    if (animName.empty()) {
      return;
    }

    if (calcPos) {
      auto [xTile, yTile] = tilePos;
      pos.first =
          xTile * TILE_WIDTH + localState.playAreaXOffset + TILE_WIDTH / 2.;
      pos.second =
          yTile * TILE_HEIGHT + localState.playAreaYOffset + TILE_HEIGHT / 2.;
    }

    if (type == PARTICLE_TEXT) {
      localState.particles.push_back(std::make_unique<Particle>(Particle{
          .animation = nullptr,
          .timer = Timer{static_cast<double>(ms)},
          .text = text,
          .x = pos.first,
          .y = pos.second,
          .ms = ms,
      }));
      return;
    }

    sdl2w::Animation* anim = new sdl2w::Animation();

    localState.particles.push_back(std::make_unique<Particle>(Particle{
        .animation = std::unique_ptr<sdl2w::Animation>(anim),
        .timer = Timer{static_cast<double>(ms)},
        .animName = animName,
        .x = pos.first,
        .y = pos.second,
        .ms = ms,
    }));
  }

public:
  static ParticleType getParticleTypeForRemove(int level) {
    std::vector<ParticleType> particleTypes = {
        PARTICLE_EXPL_0, PARTICLE_EXPL_1, PARTICLE_EXPL_2, PARTICLE_EXPL_3};
    return particleTypes[level % 4];
  }

  SpawnParticle(ParticleType type, std::pair<int, int> tilePos, int ms)
      : type(type), tilePos(tilePos), ms(ms) {

    pos = {-1, -1};
    calcPos = true;
  }

  SpawnParticle(ParticleType type, int x, int y, int ms)
      : type(type), pos(std::make_pair(x, y)), ms(ms) {}

  SpawnParticle(const std::string& text, int x, int y, int ms)
      : type(PARTICLE_TEXT), text(text), pos(std::make_pair(x, y)), ms(ms) {};
};

} // namespace actions

} // namespace program