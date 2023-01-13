#include <algorithm>
#include <iostream>

#include "utils/math_utils.h"
#include "utils/move_utils.h"
#include "backend/point.h"
#include "backend/single/single.h"

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

    Single::Single(double x, double y, const SingleStats& single_stats) : 
        x_(x), y_(y), single_state_(SingleState::STANDING), single_stats_(single_stats) {
        goal_x_ = x;
        goal_y_ = y;
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
        double distance_to_goal = math_utils::Distance(x_, y_, goal_x_, goal_y_);
        double toward_angle = atan2(goal_y_ - y_, goal_x_ - x_);

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
        vx_ = cos(angle_) * speed_;
        vy_ = sin(angle_) * speed_;
    }

    void Single::UpdateState(double time_delta) {
        // If single is already dead, cannot do anything
        if (single_state_ == SingleState::DEAD) {
            return;
        }

        // If potentially overshooting, move towards
        double delta_x = vx_ * time_delta;
        double delta_y = vy_ * time_delta;
        if (abs(goal_x_ - x_) < abs(delta_x) && abs(goal_y_ - y_) < abs(delta_y)) {
            x_ = goal_x_;
            y_ = goal_y_;
        } else {
            x_ += vx_ * time_delta;
            y_ += vy_ * time_delta;
        }
    }

    void Single::SwitchSingleState(SingleState single_state) {
        if (single_state != SingleState::DEAD) {
            single_state_ = single_state;
        }
    }

    void Single::ChangeGoalPosition(double x, double y) {
        goal_x_ = x;
        goal_y_ = y;
    }

    const Point Single::GetPosition() const {
        return { x_, y_};
    }
}