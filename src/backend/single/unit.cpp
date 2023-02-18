#include <iostream>
#include <math.h>

#include "../point.h"
#include "../variables.h"
#include "../utils/math_utils.h"
#include "single.h"
#include "unit.h"

namespace backend {
    namespace {
        void CalculatePositionOffset(std::vector<Vector2>& offset_vector, int num_singles, RNG& rng) {
            for (int i = 0; i < num_singles; i++) {
                offset_vector.push_back({
                    rng.RandDouble(-g_game_vars.single_pos_randomization, g_game_vars.single_pos_randomization),
                    rng.RandDouble(-g_game_vars.single_pos_randomization, g_game_vars.single_pos_randomization)
                });
            }
        }
    }
    Unit::Unit(Vector2 position, const SingleStats& single_stats, RNG& rng) : 
        p_(position), goal_p_(position), single_stats_(single_stats), rng_(rng){
            unit_state_ = UnitState::UNIT_STANDING;
        }

    void Unit::SetGoalPosition(Vector2 goal_p) {
        goal_p_ = goal_p;
    }

    void Unit::SwitchUnitState(UnitState unit_state) {
        unit_state_ = unit_state;
    }

    void Unit::UpdateIntention(double time_delta) {
        // Based on state, perform the movement of the unit.
        double distance_to_goal = math_utils::Distance(p_, goal_p_);
        double toward_angle = atan2(goal_p_.y - p_.y, goal_p_.x - p_.x);

        // State switching based on position
        switch (unit_state_) {
            case UnitState::UNIT_STANDING:
                if (distance_to_goal > g_game_vars.unit_standing_dist) {
                    SwitchUnitState(UnitState::UNIT_MOVING);
                }
                break;
            case UnitState::UNIT_MOVING:
                if (distance_to_goal < g_game_vars.unit_standing_dist) {
                    SwitchUnitState(UnitState::UNIT_STANDING);
                }
                break;
        }

        // Perform action based on state.
        switch (unit_state_) {
            case UnitState::UNIT_STANDING:
                break;
            case UnitState::UNIT_MOVING:
                // Wiggle the singles in the formation to simulate the randomness in walking the formation.
                position_offsets_.clear();
                CalculatePositionOffset(position_offsets_, member_singles_.size(), rng_);

                // Move towards the goal position
                v_ = {cos(toward_angle) * single_stats_.speed, sin(toward_angle) * single_stats_.speed};
                Vector2 dv = v_ * time_delta;
                if (math_utils::IsBetween(p_, p_ + dv, goal_p_)) {
                    p_ = goal_p_;
                } else {
                    p_ = p_ + dv;
                }
                break;
        }

        // Update the intention of the single the player is managing
        for (int i = 0; i < member_singles_.size(); i++) {
            Single* single = member_singles_.at(i);
            single->SetGoalPosition(p_ +  formation_offsets_.at(i) + position_offsets_.at(i));
            single->UpdateIntention(time_delta);
        }
    }

    // Absorb a single into a member of the team
    void Unit::ObtainSingle(Single* single) {
        member_singles_.push_back(single);
        std::vector<Vector2> new_offsets = math_utils::GetSunflowerFormation(g_game_vars.player_radius, member_singles_.size(), g_game_vars.single_distance_within_player);
        formation_offsets_.clear();
        formation_offsets_.insert(formation_offsets_.end(), new_offsets.begin(), new_offsets.end());
    }

    // Get member singles
    const std::vector<Single*> Unit::GetMemberSingles() const {
        return member_singles_;
    }
}