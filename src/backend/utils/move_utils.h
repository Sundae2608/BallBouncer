#pragma once

#include "../single/single.h"

namespace backend {
    namespace move_utils {
        // Rotate the angle toward a goal angle with a rotation amount.
        void RotateAngle(double* angle, double angle_goal, double rotation_amount);
    }
}