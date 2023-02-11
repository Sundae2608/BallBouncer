#include "player.h"
#include "variables.h"
#include "single/single.h"
#include "../utils/math_utils.h"

#include <math.h>
#include <random>

#include "point.h"

namespace backend {
    namespace {
        constexpr double kStandingDistance = 0.1;
        constexpr double spreadingDistance = 3;
    }

    Player::Player(uint32_t single_id, uint32_t faction_id, Vector2 position, double mass, double radius, SingleStats& single_stats, RNG& rng) :
        faction_id_(faction_id), rng_(rng) {
        main_single_ = std::make_unique<Single>(single_id, faction_id, position, mass, radius, single_stats);
    }

    void Player::UpdateIntention(double time_delta) {
        // Update the intention of the main single and each individual
        main_single_->UpdateIntention(time_delta);

        // Get the formation offset
        std::vector<Vector2> formation_offsets = math_utils::GetSunflowerFormation(main_single_->GetRadius(), member_singles_.size(), game_vars.single_distance_within_player);

        // Update the intention of the single the player is managing
        for (int i = 0; i < member_singles_.size(); i++) {
            Single* single = member_singles_.at(i);
            Vector2 formation_offset = formation_offsets.at(i);
            Vector2 position_offset = {
                rng_.RandDouble(
                    -game_vars.pos_randomization, game_vars.pos_randomization), 
                rng_.RandDouble(
                    -game_vars.pos_randomization, game_vars.pos_randomization)};
            single->SetGoalPosition(main_single_->GetPosition() + formation_offset + position_offset);
            single->UpdateIntention(time_delta);
        }
        
    }

    void Player::UpdateState(double time_delta) {
        main_single_->UpdateState(time_delta);
    }

    void Player::SetGoalPosition(Vector2 p) {
        main_single_->SetGoalPosition(p);
    }

    void Player::ObtainSingle(Single* single) {
        member_singles_.push_back(single);
    }

    Single* Player::GetSingle() {
        return main_single_.get();
    }

    const std::vector<Single*> Player::GetMemberSingles() const {
        return member_singles_;
    }

    const Vector2 Player::GetPosition() const {
        return main_single_->GetPosition();
    }

    const uint32_t Player::GetFactionId() const {
        return faction_id_;
    }
}  // namespace backend