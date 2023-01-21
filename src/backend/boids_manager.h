#pragma once

#include <unordered_set>

#include "single/single.h"
#include "collision_hasher.h"
#include "point.h"

namespace backend {
    class BoidsManager {
        public:
            BoidsManager(const CollisionHasher& collision_hasher);
            
            // Influence singles velocity using Boid rules.
            void BoidInfluence(const std::vector<Single*> singles);
        private:
            // Collision hasher
            const CollisionHasher& collision_hasher_;
    };
}  //  namespace backend