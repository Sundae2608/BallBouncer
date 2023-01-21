#include "boids_manager.h"
#include "collision_hasher.h"
#include "../utils/math_utils.h"

namespace backend {
    namespace {
        constexpr double kBoidRange = 5;
        constexpr double kBoidAvoidanceRange = 1;

        constexpr double kCentorFlockingWeight = 0.005;
        constexpr double kAvoidingBoidWeight = 1;
        constexpr double kMatchingBoidVelocityWeight = 0.1;

        Vector2 CenterFlockingRule(const Single* single, const std::vector<Single*> nearby_singles) {
            Vector2 v_com;
            for (const Single* nearby_single : nearby_singles) {
                v_com = v_com + nearby_single->GetPosition();
            }
            v_com = v_com / nearby_singles.size();
            return v_com - single->GetPosition();
        }

        Vector2 AvoidingBoidRule(const Single* single, const std::vector<Single*> nearby_singles) {
            Vector2 v_avoid;
            for (const Single* nearby_single : nearby_singles) {
                if (math_utils::WithinDistance(single->GetPosition(), nearby_single->GetPosition(), kBoidAvoidanceRange)) {
                    v_avoid = v_avoid + nearby_single->GetPosition() - single->GetPosition();
                }
            }
            return v_avoid;
        }

        Vector2 MatchingBoidVelocityRule(const Single* single, const std::vector<Single*> nearby_singles) {
            Vector2 v_matching;
            for (const Single* nearby_single : nearby_singles) {
                v_matching = v_matching + nearby_single->GetPosition();
            }
            v_matching = v_matching / nearby_singles.size();
            return v_matching;
        }
    }
        
    BoidsManager::BoidsManager(const CollisionHasher& collision_hasher) : collision_hasher_(collision_hasher) {};

    void BoidsManager::BoidInfluence(const std::vector<Single*> singles) {
        for (Single* single : singles) {
            // Get nearby singles
            const std::vector<Single*> nearby_singles = collision_hasher_.GetNearbySingles(single->GetPosition(), kBoidRange);

            // Calculate applicable singles
            std::vector<Single*> applicable_singles;
            for (Single* nearby_single : nearby_singles) {
                // Individual is not applicable if not belonging in the same faction, or is itself
                if (single->GetFactionId() != nearby_single->GetFactionId()) continue;
                if (single == nearby_single) continue;
                applicable_singles.push_back(nearby_single);
            }

            // Center flocking rule
            Vector2 v_center_flocking;
            // v_center_flocking = CenterFlockingRule(single, applicable_singles);

            // Avoiding boid rule
            Vector2 v_avoiding_boid = AvoidingBoidRule(single, applicable_singles); 

            // Matching velocity rule
            Vector2 v_matching_velocity;
            // v_matching_velocity = MatchingBoidVelocityRule(single, nearby_singles); 

            // Final velocity
            Vector2 v_modified = single->GetVelocity() + v_center_flocking + v_avoiding_boid + v_matching_velocity;
            single->SetVelocity(v_modified);
        }
    }
}