#pragma once

#include "Physics.h"
#include "game/actions/AbstractAction.h"
#include "lib/sdl2w/Animation.h"
#include "utils/Timer.hpp"
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace program {

// constexpr int TILE_WIDTH = 40;
// constexpr int TILE_HEIGHT = 33;
constexpr int TILE_WIDTH = 25;
constexpr int TILE_HEIGHT = 25;
constexpr double TILE_WIDTH_D = static_cast<double>(TILE_WIDTH);
constexpr double TILE_HEIGHT_D = static_cast<double>(TILE_HEIGHT);

struct Removable {
  int id;
  bool shouldRemove = false;
};

struct Particle {
  std::unique_ptr<sdl2w::Animation> animation;
  Timer timer;
  std::string animName;
  int x = 0;
  int y = 0;
  int ms = 0;
};

struct Bush : Removable {
  int hp = 4;
  int x = 0;
  int y = 0;
  int variant = 0;
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
struct Train : Removable {
  std::unique_ptr<Train> next;
  Timer rotationTimer = Timer{TILE_WIDTH};
  double x = 0.;
  double y = 0.;
  double prevX = 0.;
  double prevY = 0.;
  double speed = 0.;
  TrainDirectionH hDirection = TRAIN_RIGHT;
  TrainDirectionV vDirection = TRAIN_DOWN;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
  bool isHead = true;
  bool isRotating = false;
  bool isPoisoned = false;
};

struct Bomber : Removable {
  Physics physics;
  Timer shootTimer;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
  double speed = .1;
};

struct Bomb : Removable {
  Timer transformTimer;
  double x = 0;
  double y = 0;
};

struct Airplane : Removable {
  Physics physics;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
};

enum DuoMissileType {
  DUO_MISSILE_COMBINED,
  DUO_MISSILE_SINGLE,
};

struct DuoMissile : Removable {
  Physics physics;
  Timer splitTimer;
  DuoMissileType type = DUO_MISSILE_COMBINED;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
};

struct Bullet : Removable {
  Physics physics;
  int w = 2;
  int h = 10;
  double speed = .1;
};

struct PlayerControls {
  bool up = false;
  bool down = false;
  bool left = false;
  bool right = false;
  bool shoot = false;
};

struct Player {
  Physics physics{.friction = 0.03};
  PlayerControls controls;
  Timer shootTimer{16};
  double acc = 0.0065;
  int level = 0;
  int score = 0;
  int lives = 3;
  int w = TILE_WIDTH;
  int h = TILE_HEIGHT;
  bool dead = false;
  bool canShoot = false;
};

enum ControlState {
  CONTROL_MENU,
  CONTROL_IN_GAME,
  CONTROL_DEFEATED,
  CONTROL_SHOWING_HIGH_SCORE
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

  ControlState controlState = CONTROL_MENU;
  int level = 0;
  int playAreaWidthTiles = 0;
  int playAreaHeightTiles = 0;
  int playAreaXOffset = 0;
  int playAreaYOffset = 0;
  int playAreaBottomYStart = 0;
  int score = 0;
};

inline void
enqueueAction(State& state, actions::AbstractAction* action, int ms) {
  auto actionPtr =
      new actions::AsyncAction{std::unique_ptr<actions::AbstractAction>(action),
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

inline std::optional<Bullet*> findBulletByPtr(State& state, Bullet* bulletPtr) {
  for (auto& bullet : state.bullets) {
    if (bullet.get() == bulletPtr) {
      return bullet.get();
    }
  }
  return std::nullopt;
}
inline std::optional<Train*> findTrainByPtr(State& state, Train* trainPtr) {
  for (auto& train : state.trainHeads) {
    if (train.get() == trainPtr) {
      return train.get();
    }
    Train* nextTrain = train->next.get();
    while (nextTrain) {
      if (nextTrain == trainPtr) {
        return nextTrain;
      }
      nextTrain = nextTrain->next.get();
    }
  }
  LOG(WARN) << "Train not found by pointer: " << trainPtr
            << LOG_ENDL;
  return std::nullopt;
}

inline std::optional<Bomber*> findBomberByPtr(State& state, Bomber* bomberPtr) {
  for (auto& bomber : state.bombers) {
    if (bomber.get() == bomberPtr) {
      return bomber.get();
    }
  }
  return std::nullopt;
}
inline std::optional<Bomb*> findBombByPtr(State& state, Bomb* bombPtr) {
  for (auto& bomb : state.bombs) {
    if (bomb.get() == bombPtr) {
      return bomb.get();
    }
  }
  return std::nullopt;
}
inline std::optional<Airplane*> findAirplaneByPtr(State& state,
                                                  Airplane* airplanePtr) {
  for (auto& airplane : state.airplanes) {
    if (airplane.get() == airplanePtr) {
      return airplane.get();
    }
  }
  return std::nullopt;
}

} // namespace program