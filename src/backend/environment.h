#pragma once

#include "player.h"

#include <optional>
#include <random>
#include <string>
#include <unordered_map>

#include "rng.h"

namespace backend {
    class Environment {
        public:
            // Constructor and destructor
            Environment(double xl, double xu, double yl, double yu);
            ~Environment();

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

            // Random number generator
            RNG rng_;
    };
}  // namespace backend