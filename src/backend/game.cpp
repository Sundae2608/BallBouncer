#include <iostream>
#include <math.h>
#include <memory>
#include <random>

#include "backend/game.h"
#include "backend/game_config.h"
#include "backend/single/single_stats.h"

namespace backend {
    Game::Game(const GameConfig& game_config) : 
        xl_(game_config.xl), 
        xu_(game_config.xu), 
        yl_(game_config.yl), 
        yu_(game_config.yu), 
        rng_(game_config.seed),
        universal_single_stats_(game_config.single_stats) {
        
        for (int i = 0; i < game_config.num_available_singles; i++) {
            auto single = std::make_unique<Single>(rng_.RandDouble(xl_, xu_), rng_.RandDouble(yl_, yu_));
            available_singles_[single.get()] = std::move(single);
        }
    };

    Game::~Game() {
        // Destroy all players
        auto it = player_map_.begin();
        while (it != player_map_.end()) {
            it->second.reset();
            player_map_.erase(it);
        }
    }

    void Game::Update(double time_delta) {
        
        // Update intention of each player
        auto it = player_map_.begin();
        while (it != player_map_.end()) {
            it->second->UpdateIntention(time_delta);
            it++;
        }

        // Update the state of each player
        it = player_map_.begin();
        while (it != player_map_.end()) {
            it->second->UpdateState(time_delta);
            it++;
        }
    }

    void Game::AddNewPlayer(std::string player_id) {
        if (player_map_.find(player_id) == player_map_.end()) {
            player_map_[player_id] = std::make_unique<Player>(rng_.RandDouble(xl_, xu_), rng_.RandDouble(yl_, yu_), universal_single_stats_);
        }
    }

    void Game::RemovePlayer(std::string player_id) {
        if (auto it = player_map_.find(player_id); it != player_map_.end()) {
            it->second.reset();
            player_map_.erase(it);
        }
    }

    std::optional<Player*> Game::GetPlayer(std::string player_id) {
        if (auto it = player_map_.find(player_id); 
            player_map_.find(player_id) != player_map_.end()) {
            return it->second.get();
        }
        return std::nullopt;
    }
}