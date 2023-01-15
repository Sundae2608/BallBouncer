#pragma once

#include <memory>

#include "point.h"
#include "single/single.h"

namespace backend {
    class Player {
        public:
            Player(uint32_t single_id, double x, double y, SingleStats& single_stats);

            // Update the state of the player
            void UpdateState(double time_delta);

            // Update the intention of the player
            void UpdateIntention(double time_delta);

            // Update the goal of the player
            void ChangeGoalPosition(double x, double y);

            // Get player position
            const Point GetPosition() const;

            // Get player
            Single* GetSingle();

        private:
            // Singles controlled by the player
            std::unique_ptr<Single> main_single_;
    };
}