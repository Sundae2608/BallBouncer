#pragma once

#include "single/single.h"
#include "position_hasher.h"

namespace backend {
    class CollisionManager {
        public:
            CollisionManager(const PositionHasher& position_hasher);
            
            // Influence singles velocity using Boid rules.
            void ProcessCollision(double time_delta);
        private:
            // Collision hasher
            const PositionHasher& position_hasher_;
    };
}