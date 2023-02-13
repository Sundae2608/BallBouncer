#pragma once

#include <vector>

#include "../point.h"
#include "../variables.h"
#include "single.h"
#include "state.h"

namespace backend {
    class Unit {
        public:
            Unit(Vector2 position, RNG& rng);

            // Set the goal position of the unit
            void SetGoalPosition(Vector2 goal_p);

            // Update intention
            void UpdateIntention(double time_delta);

            // Update state
            void UpdateState(double time_delta);

            // Obtain single
            void ObtainSingle(Single* single);

            // Get member singles
            const std::vector<Single*> GetMemberSingles() const;
        private:
            // Singles that are under the Unit management
            std::vector<Single*> member_singles_;

            // State variables
            UnitState unit_state_;
            Vector2 p_;

            // Goal variables
            Vector2 goal_p_;

            // RNG
            RNG& rng_;
    };
}