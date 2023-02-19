#pragma once

#include <stdint.h>

#include "../rng.h"
#include "../point.h"
#include "../variables.h"
#include "state.h"

namespace backend {
    class Unit;
    class Single {
        public:
            Single(uint32_t id, uint32_t faction_id, Vector2 position, double mass, double radius, const CombatStats& combat_stats, RNG& rng);

            // Update the intention of the single
            void UpdateIntention(double time_delta);

            // Update the state of the player
            void UpdateState(double time_delta);

            // Change the faction the single belongs to
            void SwitchFaction(uint32_t faction_id);

            // Change the governing unit of the single
            void SwitchGoverningUnit(Unit* unit);

            // Gain mass
            void GainMass(double added_mass);
            
            // Getters
            Vector2 GetPosition() const;
            Vector2 GetVelocity() const;
            double GetRadius() const;
            double GetMass() const;
            double GetSpeed() const;
            uint32_t GetId() const;
            uint32_t GetFactionId() const;

            // Setters
            void SetGoalPosition(Vector2 p);
            void SetVelocity(Vector2 v);
            void SetSpeed(double speed);

        private:
            // Unique ID
            const uint32_t id_;
            uint32_t faction_id_;

            // Switch to a different state
            void SwitchSingleState(SingleState state);

            // Stats
            const CombatStats& combat_stats_;

            // Current governing unit
            Unit* governing_unit_;

            // Current state
            SingleState single_state_;
            double decision_delay_;

            // Size
            double radius_;
            double mass_;

            // Position
            Vector2 p_;
            double speed_;
            double angle_;

            // Velocity
            Vector2 v_;

            // Goal of the players
            Vector2 goal_p_;
            double goal_speed_;

            // RNG for decision making
            RNG& rng_;
    };
}