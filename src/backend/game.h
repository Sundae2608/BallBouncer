#pragma once

#include <optional>
#include <random>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "boids_manager.h"
#include "collision_manager.h"
#include "position_hasher.h"
#include "configs.h"
#include "player.h"
#include "rng.h"
#include "variables.h"

namespace backend {
    class Game {
        public:
            // Constructor and destructor
            Game(const GameConfig& game_config, const HashingConfig& hashing_config);
            ~Game();

            // Add a new player to the environment
            void AddNewPlayer(std::string player_id);

            // Remove the player from the environment
            void RemovePlayer(std::string player_id);

            // Update loop
            void Update(double time_delta);

            // Get player
            std::optional<Player*> GetPlayer(std::string player_id);

            // Get all singles
            std::vector<Single*> GetAllSingles();

            // Get all neutral singles
            std::vector<Single*> GetNeutralSingles();

            // Get all singles belong to player
            std::optional<std::vector<Single*>> GetPlayerSingles(std::string player_id);
        private:
            // Update the faction of singles
            void UpdateFaction();

            // Get a unique ID for an object in the game.
            uint32_t GetUniqueSingleId();
            uint32_t GetUniqueFactionId();

            // IDs that increment to create unique identifier for singles, factions, objects, etc
            uint32_t curr_single_id_;
            uint32_t curr_faction_id_;
            uint32_t neutral_faction_id_;

            // Lower and upper bound of the map
            double xl_;
            double xu_;
            double yl_;
            double yu_;

            // Collision hasher
            PositionHasher position_hasher_;

            // Boids manager
            BoidsManager boids_manager_;

            // Collision manager
            CollisionManager collision_manager_;

            // Map containing all the players
            std::unordered_map<std::string, std::unique_ptr<Player>> player_map_;

            // Single object up for grab
            std::unordered_map<Single*, std::unique_ptr<Single>> available_singles_;

            // Single object up for grab
            std::unordered_set<Single*> neutral_singles_;

            // Random number generator
            RNG rng_;

            // Single stats
            SingleStats universal_single_stats_;
    };
}  // namespace backend