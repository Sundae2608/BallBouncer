#include <algorithm>
#include <iostream>
#include <math.h>

#include "single.h"
#include "../utils/math_utils.h"
#include "../utils/move_utils.h"
#include "../point.h"
#include "../variables.h"

namespace backend {
    Single::Single(uint32_t id, uint32_t faction_id, Vector2 position, double mass, double radius, const SingleStats& single_stats, RNG& rng) : 
        id_(id), faction_id_(faction_id), p_(position), single_state_(SingleState::SINGLE_STANDING), 
        single_stats_(single_stats), mass_(mass), radius_(radius), rng_(rng) {
        decision_delay_ = 0;
        goal_p_ = p_;
        angle_ = 0;
    }

    void Single::UpdateIntention(double time_delta) {
        // Change the speed of the single
        if (speed_ < goal_speed_) {
            speed_ = std::min(goal_speed_, speed_ + single_stats_.acceleration * time_delta);
        } else {
            speed_ = std::max(goal_speed_, speed_ - single_stats_.acceleration * time_delta);
        }

        // Update based on states
        double distance_to_goal = math_utils::Distance(p_, goal_p_);
        double toward_angle = atan2(goal_p_.y - p_.y, goal_p_.x - p_.x);

        switch (single_state_) {
            case SingleState::SINGLE_STANDING:
                goal_speed_ = 0;
                if (distance_to_goal > g_game_vars.single_standing_dist) {
                    SwitchSingleState(SingleState::SINGLE_MOVING);
                    angle_ = toward_angle;
                }
                break;
            case SingleState::SINGLE_MOVING:
                goal_speed_ = single_stats_.speed;
                if (!math_utils::DoubleEqual(angle_, toward_angle, 1e-1) && distance_to_goal > g_game_vars.single_standing_dist) {
                    SwitchSingleState(SingleState::SINGLE_ROTATING);
                } else if (distance_to_goal < g_game_vars.single_standing_dist) {
                    SwitchSingleState(SingleState::SINGLE_STANDING);
                }
                break;
            case SingleState::SINGLE_ROTATING:
                // Rotate the angle
                goal_speed_= 0;
                move_utils::RotateAngle(&angle_, toward_angle, single_stats_.rotation_speed * time_delta);
                if (distance_to_goal > g_game_vars.single_standing_dist) {
                    if (math_utils::DoubleEqual(angle_, toward_angle, 1e-1)) {
                        SwitchSingleState(SingleState::SINGLE_MOVING);
                    }
                } else if (distance_to_goal < g_game_vars.single_standing_dist) {
                    SwitchSingleState(SingleState::SINGLE_STANDING);
                }
                break;
            case SingleState::SINGLE_DEAD:
                break;
        }

        // Calculate the intended velocity
        v_ = {cos(angle_) * speed_, sin(angle_) * speed_};
    }

    void Single::UpdateState(double time_delta) {
        // If single is already dead, cannot do anything
        if (single_state_ == SingleState::SINGLE_DEAD) {
            return;
        }

        // If potentially overshooting, just move toward the goal
        Vector2 dv = v_ * time_delta;
        if (math_utils::IsBetween(p_, p_ + dv, goal_p_)) {
            p_ = goal_p_;
        } else {
            p_ = p_ + dv;
        }
        
        // Decrement decision delay
        decision_delay_ -= time_delta;
        if (decision_delay_ > 0) {
            decision_delay_ = 0;
        }
    }

    void Single::SwitchSingleState(SingleState single_state) {
        if (single_state != SingleState::SINGLE_DEAD) {
            single_state_ = single_state;
        }
    }

    void Single::SetGoalPosition(Vector2 p) {
        goal_p_ = p;
        decision_delay_ = rng_.RandDouble(
            g_game_vars.single_decision_delay - g_game_vars.single_decision_delay_variation,
            g_game_vars.single_decision_delay + g_game_vars.single_decision_delay_variation);
    }

    void Single::SwitchFaction(uint32_t faction_id) {
        faction_id_ = faction_id;
    }
    
    void Single::GainMass(double added_mass) {
        double new_mass = mass_ + added_mass;
        radius_ = pow(new_mass / mass_, 1.0 / 3.0) * radius_;
        mass_ = new_mass;
    }

    Vector2 Single::GetPosition() const {
        return p_;
    }

    Vector2 Single::GetVelocity() const {
        return v_;
    }

    double Single::GetRadius() const {
        return radius_;
    }

    double Single::GetMass() const {
        return mass_;
    }

    double Single::GetSpeed() const {
        return speed_;
    }

    void Single::SetVelocity(Vector2 v) {
        v_ = v;
    }

    void Single::SetSpeed(double speed) {
        speed_ = speed;
    }

    uint32_t Single::GetId() const {
        return id_;
    }

    uint32_t Single::GetFactionId() const {
        return faction_id_;
    }
}