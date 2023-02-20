#include <iostream>
#include <math.h>
#include <memory>
#include <random>

#include "configs.h"
#include "game.h"
#include "player.h"
#include "variables.h"
#include "utils/math_utils.h"

namespace backend {
    Game::Game(const GameConfig& game_config, const HashingConfig& hashing_config) : 
        xl_(game_config.xl), 
        xu_(game_config.xu), 
        yl_(game_config.yl), 
        yu_(game_config.yu), 
        rng_(game_config.seed),
        universal_combat_stats_(game_config.combat_stats),
        position_hasher_(hashing_config.x_div, hashing_config.y_div),
        boids_manager_(position_hasher_),
        collision_manager_(position_hasher_),
        hitscan_manager_(position_hasher_) {
        
        // Setup id counter and neutral faction ID.
        curr_single_id_ = 0;
        neutral_faction_id_ = 0;
        curr_faction_id_ = 1;

        for (int i = 0; i < game_config.num_available_singles; i++) {
            Vector2 random_position = {rng_.RandDouble(xl_, xu_), rng_.RandDouble(yl_, yu_)};
            auto single = std::make_unique<Single>(
                GetUniqueSingleId(), neutral_faction_id_, random_position, 
                g_game_vars.single_starting_mass, g_game_vars.single_radius, universal_combat_stats_, rng_, *this);
            auto single_ptr = single.get();
            available_singles_[single.get()] = std::move(single);
            position_hasher_.AddSingle(single_ptr);
            neutral_singles_.insert(single_ptr);
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
        // Rehash all the objects
        position_hasher_.RehashObjects();

        // Update faction of each singles. If singles don't have a faction before, they will join the faction
        // that belongs to the player who gets close to them.
        UpdateFaction();
        
        // Update intention of each player and each neutral single
        for (auto it = player_map_.begin(); it != player_map_.end(); it++) {
            it->second->UpdateIntention(time_delta);
        }

        // Manage the collision
        // collision_manager_.ProcessCollision(time_delta);
        hitscan_manager_.ProcessCollision();

        // Update the state of each player
        for (auto it = player_map_.begin(); it != player_map_.end(); it++) {
            it->second->UpdateState(time_delta);
        }
        for (auto it = available_singles_.begin(); it != available_singles_.end(); it++) {
            it->first->UpdateState(time_delta);
        }
    }

    void Game::AddNewPlayer(std::string player_id) {
        if (player_map_.find(player_id) == player_map_.end()) {
            Vector2 random_position = {rng_.RandDouble(xl_, xu_), rng_.RandDouble(yl_, yu_)};
            player_map_[player_id] = std::make_unique<Player>(
                GetUniqueSingleId(), GetUniqueFactionId(), random_position, 
                g_game_vars.player_starting_mass, g_game_vars.player_radius, universal_combat_stats_, rng_, *this);
            position_hasher_.AddSingle(player_map_[player_id].get()->GetSingle());
        }
    }

    void Game::RemovePlayer(std::string player_id) {
        if (auto it = player_map_.find(player_id); it != player_map_.end()) {
            position_hasher_.RemoveSingle(it->second.get()->GetSingle());
            it->second.reset();
            player_map_.erase(it);
        }
        // TODO: Release the singles from the player back to the game
    }

    void Game::AddHitScanObject(HitscanObject bullet) {
        hitscan_manager_.AddObject(bullet);
    }

    std::optional<Player*> Game::GetPlayer(std::string player_id) {
        if (auto it = player_map_.find(player_id); 
            player_map_.find(player_id) != player_map_.end()) {
            return it->second.get();
        }
        return std::nullopt;
    }

    std::vector<Single*> Game::GetAllSingles() {
        std::vector<Single*> singles;
        auto it = available_singles_.begin();
        while (it != available_singles_.end()) 
        {
            singles.push_back(it->first);
            it++;
        }
        return singles;
    }
    
    std::vector<Single*> Game::GetNeutralSingles() {
        std::vector<Single*> singles;
        auto it = neutral_singles_.begin();
        while (it != neutral_singles_.end()) {
            singles.push_back(*it);
            it++;
        }
        return singles;
    }

    std::optional<std::vector<Single*>> Game::GetPlayerSingles(std::string player_id) {
        auto player_or = GetPlayer(player_id);
        if (player_or.has_value()) {
            return player_or.value()->GetMemberSingles();
        }
        return std::nullopt;
    }

    void Game::UpdateFaction() {
        auto it = player_map_.begin();
        while (it != player_map_.end()) {
            Player* player = it->second.get();
            const Vector2 player_pos = player->GetPosition();
            std::vector<Single*> nearby_singles = position_hasher_.GetNearbySingles(player_pos, 5);

            // Check the distance of each single.
            for (Single* single : nearby_singles) {
                const Vector2 single_pos = single->GetPosition();
                if (single->GetFactionId() == neutral_faction_id_) {
                    // Single switched to being under control of the player
                    single->SwitchFaction(player->GetFactionId());
                    single->SwitchGoverningUnit(player->GetUnit());
                    player->ObtainSingle(single);
                    neutral_singles_.erase(single);
                }
            }
            it++;
        }
    }

    uint32_t Game::GetUniqueSingleId() {
        return ++curr_single_id_;
    }

    uint32_t Game::GetUniqueFactionId() {
        return ++curr_faction_id_;
    }
}