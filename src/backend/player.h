#pragma once

#include <memory>

#include "backend/point.h"
#include "backend/single/single.h"

namespace backend {
    class Player {
        public:
            Player(double x, double y, SingleStats& single_stats);

            // Update the state of the player
            void UpdateState(double time_delta);

            // Update the intention of the player
            void UpdateIntention(double time_delta);

            // Update the goal of the player
            void ChangeGoalPosition(double x, double y);

            // Get player position
            const Point GetPosition() const;

        private:
            // Singles controlled by the player
            std::unique_ptr<Single> main_single_;
    };
}