#include "player.h"
#include "variables.h"
#include "single/single.h"
#include "single/unit.h"
#include "utils/math_utils.h"

#include <iostream>
#include <math.h>
#include <random>

#include "point.h"

namespace backend {

    Player::Player(uint32_t single_id, uint32_t faction_id, Vector2 position, double mass, double radius, CombatStats& combat_stats, RNG& rng) :
        faction_id_(faction_id), rng_(rng) {
        main_single_ = std::make_unique<Single>(single_id, faction_id, position, mass, radius, combat_stats, rng);
        unit_ = std::make_unique<Unit>(position, *this, combat_stats, rng);
    }

    void Player::UpdateIntention(double time_delta) {
        // Update the intention of the main single
        main_single_->UpdateIntention(time_delta);

        // Update the intention of each unit
        unit_->UpdateIntention(time_delta);
    }

    void Player::UpdateState(double time_delta) {
        main_single_->UpdateState(time_delta);
    }

    void Player::ObtainSingle(Single* single) {
        // Bring the single into the unit.
        unit_->ObtainSingle(single);

        // Gain mass from the eaten single
        main_single_->GainMass(single->GetMass() * g_game_vars.player_eating_mass_ratio);
    }

    void Player::MoveTo(Vector2 p) {
        main_single_->SetGoalPosition(p);
        engaging_player_ = nullptr;
        unit_->Disengage();
    }

    void Player::AttackPlayer(Player* player) {
        // Attack another player
        if (player == this) {
            // Self attacking is invalid
            return;
        }
        engaging_player_ = player;
        unit_->AttackUnit(player->GetUnit());
    }

    Single* Player::GetSingle() {
        return main_single_.get();
    }

    Unit* Player::GetUnit() {
        return unit_.get();
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