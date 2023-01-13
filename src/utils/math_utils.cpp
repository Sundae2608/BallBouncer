#include <cmath>

#include "utils/math_utils.h"

namespace math_utils {

    namespace {
        constexpr double kEpslilon = 1e-6;
    }

    bool DoubleEqual(double a, double b) {
        return abs(a - b) < kEpslilon;
    }

    bool DoubleEqual(double a, double b, double epsilon) {
        return abs(a - b) < epsilon;
    }

    bool WithinDistance(double x1, double y1, double x2, double y2, double distance) {
        double x_delta = x2 - x1;
        double y_delta = y2 - y1;
        return (x_delta * x_delta) + (y_delta * y_delta) < distance * distance;
    }

    double Distance(double x1, double y1, double x2, double y2) {
        double x_delta = x2 - x1;
        double y_delta = y2 - y1;
        return sqrt(x_delta * x_delta + y_delta * y_delta);
    }
}