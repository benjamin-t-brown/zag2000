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
  sdl2w::Window& window;
  std::unordered_map<std::string, std::unique_ptr<sdl2w::Animation>> animations;

public:
  Render(sdl2w::Window& windowA);
  void setup();

  sdl2w::Animation& getAnim(const std::string& name);
  void updateAnimations(int dt);

  void renderBush(const Bush& bush);
  void renderPlayer(const Player& player);
  void renderTrain(const Train& train);
  void renderTrainFromHead(const Train& head);
  void renderBullet(const Bullet& bullet);
  void renderBomber(const Bomber& bomber);
  void renderAirplane(const Airplane& plane);
  void renderDuoMissile(const DuoMissile& missile);
  void renderParticle(const Particle& particle);
};
} // namespace program