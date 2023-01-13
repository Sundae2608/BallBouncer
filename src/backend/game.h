#pragma once

#include <optional>
#include <random>
#include <string>
#include <map>
#include <unordered_map>

#include "backend/game_config.h"
#include "backend/player.h"
#include "backend/rng.h"
#include "backend/single/single_stats.h"

namespace backend {
    class Game {
        public:
            // Constructor and destructor
            Game(const GameConfig& game_config);
            ~Game();

            // Add a new player to the environment
            void AddNewPlayer(std::string player_id);

            // Remove the player from the environment
            void RemovePlayer(std::string player_id);

            // Update loop
            void Update(double time_delta);

            // Get player
            std::optional<Player*> GetPlayer(std::string player_id);
        private:
            // Lower and upper bound of the map
            double xl_;
            double xu_;
            double yl_;
            double yu_;

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