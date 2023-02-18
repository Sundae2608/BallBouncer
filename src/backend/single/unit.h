#pragma once

#include <vector>

#include "../point.h"
#include "../variables.h"
#include "single.h"
#include "state.h"

namespace backend {
    class Unit {
        public:
            Unit(Vector2 position, const SingleStats& single_stats, RNG& rng);

            // Set the goal position of the unit
            void SetGoalPosition(Vector2 goal_p);

            // Update intention
            void UpdateIntention(double time_delta);

            // Obtain single
            void ObtainSingle(Single* single);

            // Get member singles
            const std::vector<Single*> GetMemberSingles() const;
        private:
            // Switch the state of the unit
            void SwitchUnitState(UnitState unit_state);

            // Stats of the single
            const SingleStats& single_stats_;

            // Singles that are under the Unit management
            std::vector<Single*> member_singles_;

            // Offset position of each member, compared to the center of the unit.
            std::vector<Vector2> formation_offsets_;
            std::vector<Vector2> position_offsets_;

            // State variables
            UnitState unit_state_;
            Vector2 p_;
            Vector2 v_;

            // Goal variables
            Vector2 goal_p_;

            // RNG
            RNG& rng_;
    };
}