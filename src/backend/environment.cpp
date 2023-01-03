#include "environment.h"

#include <math.h>
#include <memory>
#include <random>

namespace backend {
    Environment::Environment(double xl, double xu, double yl, double yu) : 
        xl_(xl), xu_(xu), yl_(yl), yu_(yu), rng_(42) { };

    Environment::~Environment() {
        // Destroy all players
        auto it = player_map_.begin();
        while (it != player_map_.end()) {
            it->second.reset();
            player_map_.erase(it);
        }
    }

    void Environment::Update(double time_delta) {
        
        // Update intention of each player
        auto it = player_map_.begin();
        while (it != player_map_.end()) {
            it->second->UpdateIntention();
        }

        // Update the state of each player
        it = player_map_.begin();
        while (it != player_map_.end()) {
            it->second->UpdateState(time_delta);
        }
    }

    void Environment::AddNewPlayer(std::string player_id) {
        if (player_map_.find(player_id) == player_map_.end()) {
            player_map_[player_id] = std::make_unique<Player>(rng_.RandDouble(xl_, xu_), rng_.RandDouble(yl_, yu_));
        }
    }

    void Environment::RemovePlayer(std::string player_id) {
        if (auto it = player_map_.find(player_id); it != player_map_.end()) {
            it->second.reset();
            player_map_.erase(it);
        }
    }

    std::optional<Player*> Environment::GetPlayer(std::string player_id) {
        if (auto it = player_map_.find(player_id); 
            player_map_.find(player_id) != player_map_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
}