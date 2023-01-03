#pragma once

namespace backend {
    class Player {
        public:
            Player(double x, double y);

            // Update the state of the player
            void UpdateState(double time_delta);

            // Update the intention of the player
            void UpdateIntention();

            // Update the goal of the player
            void ChangeGoalPosition(double x, double y);
        private:
            // Stats
            double speed_ = 5;

            // Position
            double x_;
            double y_;

            // Velocity
            double vx_;
            double vy_;

            // Goal of the player
            double goal_x_;
            double goal_y_;
    };
}