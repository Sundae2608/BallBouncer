#pragma once

#include <vector>

#include "../point.h"
#include "../single/single.h"

namespace backend {
    namespace math_utils {

        // Compare if the two double number equals each other given the tolerance window epsilon
        bool DoubleEqual(double a, double b, double epsilon);

        // Compare if the two double number equals each other. The tolerance is 1e-6.
        bool DoubleEqual(double a, double b);

        // Distance calculation
        double Distance(Vector2 p1, Vector2 p2);

        // Check whether the distance between p1 and p2 is shorter than input distance
        bool WithinDistance(Vector2 p1, Vector2 p2, double distance);

        // Check whether point a is between point b and point p
        bool IsBetween(Vector2 a, Vector2 b, Vector2 p);

        // Return the sunflower seed formation based on the radius, number of seeds and the spreading distance
        std::vector<Vector2> GetSunflowerFormation(double radius, int num_seeds, double spreading_distance);

        // Sort singles by angle
        std::vector<Single*> SortSinglesByAngle(std::vector<Single*> singles, double angle);

        // Check whether the line segment from p1 and p2 intersects the circle defined by center and radius.
        bool LineCircleIntersection(Vector2 p1, Vector2 p2, Vector2 center, double radius);

        // Convert radial co-ordinate to x-y coordinate
        Vector2 GetCartesianVector(double r, double theta);
    }
}