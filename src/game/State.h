#pragma once

#include "Physics.h"
#include "game/actions/AbstractAction.h"
#include "lib/sdl2w/Animation.h"
#include "utils/Timer.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace program {

// constexpr int TILE_WIDTH = 40;
// constexpr int TILE_HEIGHT = 33;
constexpr int TILE_WIDTH = 25;
constexpr int TILE_HEIGHT = 25;

struct Particle {
  std::unique_ptr<sdl2w::Animation> animation;
  int x = 0;
  int y = 0;
};

struct Bush {
  int hp = 4;
  int x = 0;
  int y = 0;
  bool marked = false;
};

enum TrainDirectionV {
  TRAIN_UP,
  TRAIN_DOWN,
};
enum TrainDirectionH {
  TRAIN_LEFT,
  TRAIN_RIGHT,
};
struct Train {
  std::unique_ptr<Train> next;
  Timer rotationTimer = Timer{TILE_WIDTH};
  double x = 0.;
  double y = 0.;
  double prevX = 0.;
  double prevY = 0.;
  double speed = .1;
  TrainDirectionH hDirection = TRAIN_RIGHT;
  TrainDirectionV vDirection = TRAIN_DOWN;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
  bool isHead = true;
  bool isRotating = false;
};

struct Bomber {
  Physics physics;
  Timer shootTimer;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
  double speed = .1;
};

struct Bomb {
  Timer transformTimer;
  double x = 0;
  double y = 0;
};

struct Airplane {
  Physics physics;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
};

enum DuoMissileType {
  DUO_MISSILE_COMBINED,
  DUO_MISSILE_SINGLE,
};

struct DuoMissile {
  Physics physics;
  Timer splitTimer;
  DuoMissileType type = DUO_MISSILE_COMBINED;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
};

struct Bullet {
  Physics physics;
  int w = 2;
  int h = 10;
  double speed = .1;
};

struct Player {
  Physics physics;
  int level = 0;
  int score = 0;
  int lives = 3;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
  bool dead = false;
  bool canShoot = false;
};

struct State {
  std::unordered_map<int, std::unique_ptr<Bush>> bushes;
  std::vector<std::unique_ptr<Train>> trainHeads;
  std::vector<std::unique_ptr<Bullet>> bullets;
  std::vector<std::unique_ptr<Bomber>> bombers;
  std::vector<std::unique_ptr<Bomb>> bombs;
  std::vector<std::unique_ptr<Airplane>> airplanes;
  std::vector<std::unique_ptr<DuoMissile>> duoMissiles;
  std::vector<std::unique_ptr<Particle>> particles;
  std::vector<std::unique_ptr<actions::AsyncAction>> sequentialActions;
  std::vector<std::unique_ptr<actions::AsyncAction>> sequentialActionsNext;
  std::vector<std::unique_ptr<actions::AsyncAction>> parallelActions;
  std::vector<std::string> soundsToPlay;
  Player player;

  int playAreaWidthTiles = 0;
  int playAreaHeightTiles = 0;
  int playAreaXOffset = 0;
  int playAreaYOffset = 0;
};

inline void enqueueAction(State& state,
                          std::unique_ptr<actions::AbstractAction> action,
                          int ms) {
  auto actionPtr = new actions::AsyncAction{std::move(action),
                                            Timer{static_cast<double>(ms), 0}};
  state.sequentialActionsNext.push_back(
      std::unique_ptr<actions::AsyncAction>(actionPtr));
}

inline void addParallelAction(State& state,
                              std::unique_ptr<actions::AbstractAction> action,
                              int ms) {
  state.parallelActions.push_back(
      std::unique_ptr<actions::AsyncAction>(new actions::AsyncAction{
          std::move(action), Timer{static_cast<double>(ms), 0}}));
}

inline void moveSequentialActions(State& state) {
  state.sequentialActions.insert(
      state.sequentialActions.end(),
      std::make_move_iterator(state.sequentialActionsNext.begin()),
      std::make_move_iterator(state.sequentialActionsNext.end()));
  state.sequentialActionsNext.clear();
}

} // namespace program