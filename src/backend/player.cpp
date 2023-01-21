#include "player.h"
#include "single/single.h"
#include "../utils/math_utils.h"

#include <math.h>
#include <random>

#include "point.h"

namespace backend {
    namespace {
        constexpr double kStandingDistance = 0.1;
    }

    Player::Player(uint32_t single_id, uint32_t faction_id, double x, double y, SingleStats& single_stats) :
        faction_id_(faction_id) {
        main_single_ = std::make_unique<Single>(single_id, faction_id, x, y, single_stats);
    }

    void Player::UpdateIntention(double time_delta) {
        // Update the intention of the main single and each individual
        main_single_->UpdateIntention(time_delta);

        // Update the intention of the single the player is managing
        for (Single* single : member_singles_) {
            single->SetGoalPosition(main_single_->GetPosition());
            single->UpdateIntention(time_delta);
        }
        
    }

    void Player::UpdateState(double time_delta) {
        main_single_->UpdateState(time_delta);
    }

    void Player::SetGoalPosition(double x, double y) {
        main_single_->SetGoalPosition(x, y);
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