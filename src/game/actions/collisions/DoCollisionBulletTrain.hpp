#pragma once

#include "game/State.h"
#include "game/actions/AbstractAction.h"
#include "game/actions/spawns/SpawnBush.hpp"
#include "game/actions/spawns/SpawnParticle.hpp"
#include "game/updaters/UpdateTrain.h"
#include <utility>

namespace program {

namespace actions {

class DoCollisionBulletTrain : public AbstractAction {
  Bullet* bulletPtr;
  Train* trainPtr;

  void act() override {
    State& localState = *this->state;
    if (findBulletByPtr(localState, bulletPtr).has_value()) {
      bulletPtr->shouldRemove = true;
    }
    if (findTrainByPtr(localState, trainPtr).has_value()) {
      trainPtr->shouldRemove = true;

      int ind = getTrainLookAheadInd(localState, *trainPtr);
      if (ind > 0) {
        int x = ind % localState.playAreaWidthTiles;
        int y = ind / localState.playAreaWidthTiles;
        enqueueAction(localState, new SpawnBush(std::make_pair(x, y)), 0);
      }
      int trainInd = getTrainInd(localState, *trainPtr);
      if (trainInd > 0) {
        int x = trainInd % localState.playAreaWidthTiles;
        int y = trainInd / localState.playAreaWidthTiles;
        enqueueAction(
            localState,
            new SpawnParticle(ParticleType::PARTICLE_EXPL_0, {x, y}, 200), 0);
      }
      if (trainPtr->next) {
        trainPtr->next->isHead = true;
        state->trainHeads.push_back(std::move(trainPtr->next));
        trainPtr->next = nullptr;
      }

      localState.score += 10;
    }
  }

public:
  DoCollisionBulletTrain(Bullet* bullet, Train* train)
      : bulletPtr(bullet), trainPtr(train) {}
};

} // namespace actions

} // namespace program