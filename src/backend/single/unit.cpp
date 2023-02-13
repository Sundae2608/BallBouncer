#include "../point.h"
#include "../../utils/math_utils.h"
#include "single.h"
#include "unit.h"

namespace backend {
    Unit::Unit(Vector2 position, RNG& rng) : 
        p_(position), goal_p_(position), rng_(rng){}

    void Unit::SetGoalPosition(Vector2 goal_p) {
        goal_p_ = goal_p;
    }

    void Unit::UpdateIntention(double time_delta) {
        // For now, intention simply matches the goal
        p_ = goal_p_;

        // Get the formation offset
        std::vector<Vector2> formation_offsets = math_utils::GetSunflowerFormation(g_game_vars.player_radius, member_singles_.size(), g_game_vars.single_distance_within_player);

        // Update the intention of the single the player is managing
        for (int i = 0; i < member_singles_.size(); i++) {
            Single* single = member_singles_.at(i);
            Vector2 formation_offset = formation_offsets.at(i);
            Vector2 position_offset = {
                rng_.RandDouble(
                    -g_game_vars.single_pos_randomization, g_game_vars.single_pos_randomization), 
                rng_.RandDouble(
                    -g_game_vars.single_pos_randomization, g_game_vars.single_pos_randomization)};
            single->SetGoalPosition(p_ + formation_offset + position_offset);
            single->UpdateIntention(time_delta);
        }
    }

    void Unit::UpdateState(double time_delta) {
        
    }

    // Absorb a single into a member of the team
    void Unit::ObtainSingle(Single* single) {
        member_singles_.push_back(single);
    }

    // Get member singles
    const std::vector<Single*> Unit::GetMemberSingles() const {
        return member_singles_;
    }
}