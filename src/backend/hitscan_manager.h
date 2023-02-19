#pragma once

#include "single/single.h"
#include "point.h"
#include "position_hasher.h"

namespace backend {
    struct HitscanObject {
        Vector2 p_source;
        double angle;
        int faction_id;
    };

    class HitscanManager {
        public:
            HitscanManager(const PositionHasher& position_hasher);
            
            // Influence singles velocity using Boid rules.
            void ProcessCollision();

            // Add hitscan object
            void AddObject(HitscanObject& object);
        private:
            // Collision hasher
            const PositionHasher& position_hasher_;

            // Hitscan object to be processed
            std::vector<HitscanObject> hitscan_objects_;
    };
}