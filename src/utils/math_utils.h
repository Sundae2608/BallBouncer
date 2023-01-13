#pragma once

namespace math_utils {

    // Compare if the two double number equals each other given the tolerance window epsilon
    bool DoubleEqual(double a, double b, double epsilon);

    // Compare if the two double number equals each other. The tolerance is 1e-6.
    bool DoubleEqual(double a, double b);

    // Distance calculation
    double Distance(double x1, double y1, double x2, double y2);

    // Check whether the distance between (x1, y1) and (x2, y2) is shorter than input distance
    bool WithinDistance(double x1, double y1, double x2, double y2, double distance);

}