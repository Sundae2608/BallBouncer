#pragma once

#include <memory>
#include <vector>

#include "point.h"
#include "rng.h"
#include "single/single.h"
#include "single/unit.h"

namespace backend {
    class Player {
        public:
            Player(uint32_t single_id, uint32_t faction_id, Vector2 position, double mass, double radius, CombatStats& combat_stats, RNG& rng);

            // Update the state of the player
            void UpdateState(double time_delta);

            // Update the intention of the player
            void UpdateIntention(double time_delta);

            // Update the goal of the player
            void MoveTo(Vector2 p);

            // Absorb a single into a member of the team
            void ObtainSingle(Single* single);

            // Attack another player
            void AttackPlayer(Player* player);

            // Get player
            Single* GetSingle();

            // Get main unit
            Unit* GetUnit();

            // Get controlling singles
            const std::vector<Single*> GetMemberSingles() const;

            // Get player position
            const Vector2 GetPosition() const;

            // Get player faction
            const uint32_t GetFactionId() const;

        private:
            // Faction represented by the player
            uint32_t faction_id_;

            // Engaged player
            Player* engaging_player_;

            // Main single controlled by the player
            std::unique_ptr<Single> main_single_;
            std::unique_ptr<Unit> unit_;

            // RNG
            RNG& rng_;
    };
}