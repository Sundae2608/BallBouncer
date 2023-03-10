#pragma once

#include "single/single.h"
#include "position_hasher.h"

namespace backend {
    class BoidsManager {
        public:
            BoidsManager(const PositionHasher& position_hasher);
            
            // Influence singles velocity using Boid rules.
            void BoidInfluence(const std::vector<Single*> singles);
        private:
            // Collision hasher
            const PositionHasher& position_hasher_;
    };
}  //  namespace backend