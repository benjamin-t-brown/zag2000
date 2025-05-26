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
};

class SpawnParticle : public AbstractAction {
  ParticleType type;
  std::pair<int, int> tilePos;
  int ms;

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

    auto [xTile, yTile] = tilePos;
    const int x =
        xTile * TILE_WIDTH + localState.playAreaXOffset + TILE_WIDTH / 2.;
    const int y =
        yTile * TILE_HEIGHT + localState.playAreaYOffset + TILE_HEIGHT / 2.;

    sdl2w::Animation* anim = new sdl2w::Animation();

    state->particles.push_back(std::make_unique<Particle>(Particle{
        .animation = std::unique_ptr<sdl2w::Animation>(anim),
        .timer = Timer{static_cast<double>(ms)},
        .animName = animName,
        .x = x,
        .y = y,
        .ms = ms,
    }));
  }

public:
  SpawnParticle(ParticleType type, std::pair<int, int> tilePos, int ms)
      : type(type), tilePos(tilePos), ms(ms) {}
};

} // namespace actions

} // namespace program