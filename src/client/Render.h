#pragma once

#include "lib/sdl2w/Animation.h"
#include <memory>
#include <string>
#include <unordered_map>
#include "game/State.h"

namespace sdl2w {
class Window;
}

namespace program {
class Render {
  State* statePtr = nullptr;
  sdl2w::Window& window;
  std::unordered_map<std::string, std::unique_ptr<sdl2w::Animation>> animations;

public:
  Render(sdl2w::Window& windowA);
  void setup(State& state);
  const State& getState() const { return *statePtr; }

  sdl2w::Animation& getAnim(const std::string& name);
  void updateAnimations(int dt);

  int getSpriteOffsetLevel(int offset = 0);
  void renderBg();
  void renderBush(const Bush& bush);
  void renderPlayer(const Player& player);
  void renderTrain(const Train& train);
  void renderTrainFromHead(const Train& head);
  void renderBullet(const Bullet& bullet);
  void renderBomber(const Bomber& bomber);
  void renderBomb(const Bomb& bomb);
  void renderAirplane(const Airplane& plane);
  void renderDuoMissile(const DuoMissile& missile);
  void renderParticle(const Particle& particle);
  void renderCollisionCircle(const CollisionCircle& circle);
  void renderUi();
};
} // namespace program