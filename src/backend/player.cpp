#include "player.h"
#include "variables.h"
#include "single/single.h"
#include "single/unit.h"
#include "../utils/math_utils.h"

#include <math.h>
#include <random>

#include "point.h"

namespace backend {

    Player::Player(uint32_t single_id, uint32_t faction_id, Vector2 position, double mass, double radius, SingleStats& single_stats, RNG& rng) :
        faction_id_(faction_id), rng_(rng) {
        main_single_ = std::make_unique<Single>(single_id, faction_id, position, mass, radius, single_stats, rng);
        unit_ = std::make_unique<Unit>(position, single_stats, rng);
    }

    void Player::UpdateIntention(double time_delta) {
        // Update the intention of the main single
        main_single_->UpdateIntention(time_delta);

        // Update the intention of each unit
        unit_->SetGoalPosition(main_single_->GetPosition());
        unit_->UpdateIntention(time_delta);
    }

    void Player::UpdateState(double time_delta) {
        main_single_->UpdateState(time_delta);
    }

    void Player::SetGoalPosition(Vector2 p) {
        main_single_->SetGoalPosition(p);
    }

    void Player::ObtainSingle(Single* single) {
        // Bring the single into the unit.
        unit_->ObtainSingle(single);

        // Gain mass from the eaten single
        main_single_->GainMass(single->GetMass() * g_game_vars.player_eating_mass_ratio);
    }

    Single* Player::GetSingle() {
        return main_single_.get();
    }

    const std::vector<Single*> Player::GetMemberSingles() const {
        return unit_->GetMemberSingles();
    }

    const Vector2 Player::GetPosition() const {
        return main_single_->GetPosition();
    }

    const uint32_t Player::GetFactionId() const {
        return faction_id_;
    }
}  // namespace backend