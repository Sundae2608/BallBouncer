#pragma once

#include <vector>

#include "../backend/point.h"

namespace math_utils {

    // Compare if the two double number equals each other given the tolerance window epsilon
    bool DoubleEqual(double a, double b, double epsilon);

    // Compare if the two double number equals each other. The tolerance is 1e-6.
    bool DoubleEqual(double a, double b);

    // Distance calculation
    double Distance(backend::Vector2 p1, backend::Vector2 p2);

    // Check whether the distance between (x1, y1) and (x2, y2) is shorter than input distance
    bool WithinDistance(backend::Vector2 p1, backend::Vector2 p2, double distance);

    // Return the sunflower seed formation based on the radius, number of seeds and the spreading distance
    std::vector<backend::Vector2> GetSunflowerFormation(double radius, int num_seeds, double spreading_distance);
}