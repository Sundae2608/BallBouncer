#include <algorithm>
#include <iostream>

#include "single.h"
#include "../../utils/math_utils.h"
#include "../../utils/move_utils.h"
#include "../point.h"

namespace backend {
    namespace  {
        // Distance to the goal in which the agent should be considered STANDING.
        constexpr double kStandingDist = 0.1;

        // The coefficient of the out of reach distance.
        // When multiplied with the inherent speed, the result represents the distance over which
        // single is considered out of position and need to catch up. In that case, the speed in which
        // the agent run will be multiplied to catch up.
        constexpr double kOutOfReachCoefficient = 5;
        constexpr double kOutOfReachSpeedMultiplier = 1.4;
    }

    Single::Single(uint32_t id, uint32_t faction_id, Vector2 position, double mass, double radius, const SingleStats& single_stats) : 
        id_(id), faction_id_(faction_id), p_(position), single_state_(SingleState::STANDING), single_stats_(single_stats), mass_(mass), radius_(radius) {
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
            case SingleState::STANDING:
                goal_speed_ = 0;
                if (distance_to_goal > kStandingDist) {
                    SwitchSingleState(SingleState::MOVING);
                    angle_ = toward_angle;
                }
                break;
            case SingleState::MOVING:
                goal_speed_ = single_stats_.speed;
                if (!math_utils::DoubleEqual(angle_, toward_angle, 1e-1) && distance_to_goal > kStandingDist) {
                    SwitchSingleState(SingleState::ROTATING);
                } else if (distance_to_goal < kStandingDist) {
                    SwitchSingleState(SingleState::STANDING);
                }
                break;
            case SingleState::ROTATING:
                // Rotate the angle
                goal_speed_= speed_;
                move_utils::RotateAngle(&angle_, toward_angle, single_stats_.rotation_speed * time_delta);
                if (distance_to_goal > kStandingDist) {
                    if (math_utils::DoubleEqual(angle_, toward_angle, 1e-1)) {
                        SwitchSingleState(SingleState::MOVING);
                    }
                } else if (distance_to_goal < kStandingDist) {
                    SwitchSingleState(SingleState::STANDING);
                }
                break;
            case SingleState::DEAD:
                break;
        }

        // Calculate the intended velocity
        v_ = {cos(angle_) * speed_, sin(angle_) * speed_};
    }

    void Single::UpdateState(double time_delta) {
        // If single is already dead, cannot do anything
        if (single_state_ == SingleState::DEAD) {
            return;
        }

        // If potentially overshooting, move towards
        Vector2 dv = v_ * time_delta;
        if (goal_p_.x - p_.x < abs(dv.x) && abs(goal_p_.y - p_.y) < abs(dv.y)) {
            p_ = goal_p_;
        } else {
            p_ = p_ + v_ * time_delta;
        }
    }

    void Single::SwitchSingleState(SingleState single_state) {
        if (single_state != SingleState::DEAD) {
            single_state_ = single_state;
        }
    }

    void Single::SetGoalPosition(Vector2 p) {
        goal_p_ = p;
    }

    void Single::SwitchFaction(uint32_t faction_id) {
        faction_id_ = faction_id;
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