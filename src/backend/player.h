#pragma once

#include <memory>
#include <vector>

#include "point.h"
#include "rng.h"
#include "single/single.h"

namespace backend {
    class Player {
        public:
            Player(uint32_t single_id, uint32_t faction_id, Vector2 position, double mass, double radius, SingleStats& single_stats, RNG& rng);

            // Update the state of the player
            void UpdateState(double time_delta);

            // Update the intention of the player
            void UpdateIntention(double time_delta);

            // Update the goal of the player
            void SetGoalPosition(Vector2 p);

            // Absorb a single into a member of the team
            void ObtainSingle(Single* single);

            // Get player
            Single* GetSingle();

            // Get controlling singles
            const std::vector<Single*> GetMemberSingles() const;

            // Get player position
            const Vector2 GetPosition() const;

            // Get player faction
            const uint32_t GetFactionId() const;

        private:
            // Faction represented by the player
            uint32_t faction_id_;

            // Main single controlled by the player
            std::unique_ptr<Single> main_single_;

            // Other single controlled by the player
            std::vector<Single*> member_singles_;

            // RNG
            RNG& rng_;
    };
}