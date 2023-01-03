#include "player.h"

#include <math.h>
#include <random>

namespace backend {
    Player::Player(double x, double y) : x_(x), y_(y) {};

    void Player::UpdateState(double time_delta) {
        x_ += time_delta * vx_;
        y_ += time_delta * vy_;
    }

    void Player::UpdateIntention() {
        double angle = atan2(goal_y_ - y_, goal_x_ - x_);
        double vx = speed_ * cos(angle);
        double vy = speed_ * sin(angle);
    }

    void Player::ChangeGoalPosition(double x, double y) {
        goal_x_ = x;
        goal_y_ = y;
    }
}  // namespace backend