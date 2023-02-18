#include <cmath>
#include <functional>
#include <vector>
#include <unordered_map>

#include "../backend/point.h"
#include "math_utils.h"

namespace backend {
    namespace math_utils {

        namespace {
            constexpr double kEpslilon = 1e-6;
            constexpr double kGoldenAngle = 2.39996322972865332;
            
            struct SunflowerKey {
                double radius;
                int num_seeds;
                double spreading_distance;

                bool operator==(const SunflowerKey& other) const {
                    return radius == other.radius && num_seeds == other.num_seeds && spreading_distance && other.spreading_distance;
                }
            };

            struct SunflowerKeyHash {
                std::size_t operator()(const SunflowerKey& k) const {
                    return std::hash<double>()(k.radius) ^ std::hash<int>()(k.num_seeds) ^ std::hash<double>()(k.spreading_distance);
                }
            };

            static std::unordered_map<SunflowerKey, std::vector<Vector2>, SunflowerKeyHash> kSunflowerFormationMemo;
        }


        bool DoubleEqual(double a, double b) {
            return abs(a - b) < kEpslilon;
        }

        bool DoubleEqual(double a, double b, double epsilon) {
            return abs(a - b) < epsilon;
        }

        bool WithinDistance(Vector2 p1, Vector2 p2, double distance) {
            double x_delta = p2.x - p1.x;
            double y_delta = p2.y - p1.y;
            return (x_delta * x_delta) + (y_delta * y_delta) < distance * distance;
        }

        double Distance(Vector2 p1, Vector2 p2) {
            return sqrt((p2 - p1).SquareMagnitude());
        }
        
        bool IsBetween(Vector2 a, Vector2 b, Vector2 p) {
            double ap = Distance(a, p);
            double bp = Distance(b, p);
            double ab = Distance(a, b);

            return abs(ab - bp - ap) < kEpslilon;
        }

        bool LineCircleIntersection(Vector2 p1, Vector2 p2, Vector2 center, double radius) {
            double dx = p2.x - p1.x;
            double dy = p2.y - p1.y;
            double fx = p1.x - center.x;
            double fy = p1.y - center.y;

            double a = dx*dx + dy*dy;
            double b = 2*(fx*dx + fy*dy);
            double c = fx*fx + fy*fy - radius*radius;

            double discriminant = b*b-4*a*c;
            if (discriminant < 0) {
                return 0;
            } else {
                discriminant = sqrt(discriminant);
                double t1 = (-b - discriminant) / (2*a);
                double t2 = (-b + discriminant) / (2*a);
                if (t1 >= 0 && t1 <= 1) {
                    return true;
                } else if (t2 >= 0 && t2 <= 1) {
                    return true;
                } else {
                    return false;
                }
            }
        }

        std::vector<Vector2> GetSunflowerFormation(double radius, int num_seeds, double spreading_distance) {
            // Construct the look up key
            SunflowerKey lookup_key = {radius, num_seeds, spreading_distance};

            // Look up case
            if (kSunflowerFormationMemo.find(lookup_key) != kSunflowerFormationMemo.end()) {
                return kSunflowerFormationMemo[lookup_key];
            }

            // Construct the formation
            std::vector<Vector2> return_vec;
            int drawn_seeds = 0;
            int i = (radius / spreading_distance) * (radius / spreading_distance);
            while (true) {
                i += 1;
                double r = spreading_distance * sqrt(i);
                if (r < radius) {
                    continue;
                }
                double theta = i * kGoldenAngle;
                double x = r * cos(theta);
                double y = r * sin(theta);
                return_vec.push_back({x, y});
                drawn_seeds += 1;
                if (drawn_seeds > num_seeds) {
                    break;
                }
            }
            
            // Store the results into the memo for future lookup, and return the results
            kSunflowerFormationMemo[lookup_key] = return_vec;
            return return_vec;
        }
    }
}