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
        main_single_->UpdateIntention(time_delta);
    }

    void Player::UpdateState(double time_delta) {
        main_single_->UpdateState(time_delta);
    }

    void Player::ChangeGoalPosition(double x, double y) {
        main_single_->ChangeGoalPosition(x, y);
    }

    Single* Player::GetSingle() {
        return main_single_.get();
    }

    const Point Player::GetPosition() const {
        return main_single_->GetPosition();
    }

    const uint32_t Player::GetFactionId() const {
        return faction_id_;
    }
}  // namespace backend