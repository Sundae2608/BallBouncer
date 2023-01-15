#pragma once

#include "../point.h"
#include "single_state.h"
#include "single_stats.h"

namespace backend {
    class Single {
        public:
            Single(uint32_t id, uint32_t faction_id, double x, double y, const SingleStats& single_stats_);

            // Update the intention of the single
            void UpdateIntention(double time_delta);

            // Update the state of the player
            void UpdateState(double time_delta);

            // Set the goal position of the single
            void ChangeGoalPosition(double x, double y);

            // Change the faction the single belongs to
            void SwitchFaction(uint32_t faction_id);
            
            // Get the position of the single
            const Point GetPosition() const;

            // Get ID of the single
            const uint32_t GetId() const;

            // Get faction ID of the single
            const uint32_t GetFactionId() const;

        private:
            // Unique ID
            uint32_t id_;
            uint32_t faction_id_;

            // Switch to a different state
            void SwitchSingleState(SingleState state);

            // Stats
            const SingleStats& single_stats_;

            // Current state
            SingleState single_state_;

            // Position
            double x_;
            double y_;
            double speed_;
            double angle_;

            // Velocity
            double vx_;
            double vy_;

            // Goal of the players
            double goal_x_;
            double goal_y_;
            double goal_speed_;
    };
}