#pragma once

#include <optional>
#include <random>
#include <string>
#include <map>
#include <unordered_map>

#include "collision_hasher.h"
#include "configs.h"
#include "player.h"
#include "rng.h"
#include "single/single_stats.h"

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
        private:
            // Lower and upper bound of the map
            double xl_;
            double xu_;
            double yl_;
            double yu_;

            // Collision hasher
            CollisionHasher collision_hasher_;

            // Map containing all the players
            std::unordered_map<std::string, std::unique_ptr<Player>> player_map_;

            // Single object up for grab
            std::unordered_map<Single*, std::unique_ptr<Single>> available_singles_;

            // Random number generator
            RNG rng_;

            // Single stats
            SingleStats universal_single_stats_;
    };
}  // namespace backend