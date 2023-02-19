#pragma once

#include <optional>
#include <vector>

#include "../point.h"
#include "../variables.h"
#include "single.h"
#include "state.h"

namespace backend {
    class Player;
    class Unit {
        public:
            Unit(Vector2 position, const Player& player, const CombatStats& combat_stats, RNG& rng);

            // Set the goal position of the unit
            void SetGoalPosition(Vector2 goal_p);

            // Get position
            Vector2 GetPosition();

            // Update intention
            void UpdateIntention(double time_delta);

            // Obtain single
            void ObtainSingle(Single* single);

            // Attack the unit
            void AttackUnit(Unit* unit);
            void Disengage();

            // Get engaging unit
            Unit* GetEngagingUnit();

            // Whether unit is engaging
            UnitState GetState();

            // Get member singles
            const std::vector<Single*> GetMemberSingles() const;
        private:
            // Switch the state of the unit
            void SwitchUnitState(UnitState unit_state);

            // Stats of the single
            const CombatStats& combat_stats_;

            // Singles that are under the Unit management
            const Player& player_;
            std::vector<Single*> member_singles_;

            // Offset position of each member, compared to the center of the unit.
            std::vector<Vector2> formation_offsets_;
            std::vector<Vector2> position_offsets_;

            // State variables
            UnitState unit_state_;
            Vector2 p_;
            Vector2 v_;
            double toward_angle_;
            Unit* engaging_unit_;

            // Goal variables
            Vector2 goal_p_;

            // RNG
            RNG& rng_;
    };
}