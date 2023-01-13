#include <cmath>
#include <math.h>

#include "utils/move_utils.h"


namespace move_utils {
    namespace {
        constexpr double kPi = 3.14159265358979323846;
        constexpr double k2Pi = 2 * kPi;
    }
    
    // Rotate the angle toward a goal angle with a rotation amount.
    void RotateAngle(double* angle, double angle_goal, double rotation_amount) {
        // First, calculate the signed angle
        double angle_diff = angle_goal - *angle;
        if (angle_diff > kPi) angle_diff -= k2Pi;
        else if (angle_diff < - kPi) angle_diff += k2Pi;

        if (abs(angle_diff) < rotation_amount) *angle = angle_goal;
        else if (angle_diff < 0) *angle -= rotation_amount;
        else *angle += rotation_amount;

        if (*angle > kPi) *angle -= k2Pi;
        else if (*angle < - kPi) *angle += k2Pi;
    }
}