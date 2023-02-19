#include "collision_manager.h"
#include "position_hasher.h"

namespace backend {
    CollisionManager::CollisionManager(const PositionHasher& position_hasher) : position_hasher_(position_hasher) {};

    void CollisionManager::ProcessCollision(double time_delta) {
        // Get all singles to process the collision
        std::vector<Single*> singles = position_hasher_.GetAllSingles();

        // For each single, consider the nearby singles
        for (Single* single : singles) {
            const std::vector<Single*> nearby_singles = position_hasher_.GetNearbySingles(single->GetPosition());
            Vector2 a_total = {0.0, 0.0};

            for (Single* nearby_single : nearby_singles) {
                Vector2 diff_vector = nearby_single->GetPosition() - single->GetPosition();
                double square_dist = diff_vector.SquareMagnitude();
                double min_dist = single->GetRadius() + nearby_single->GetRadius();
                double min_square_dist = min_dist * min_dist;

                // Calculate deflection
                if (square_dist < min_square_dist) {
                    double angle = atan2(diff_vector.y, diff_vector.x);
                    Vector2 target;
                    target.x = single->GetPosition().x + cos(angle) * min_dist;
                    target.y = single->GetPosition().y + sin(angle) * min_dist;
                    Vector2 a = (target - nearby_single->GetPosition()) * g_physics_stats.spring_push * nearby_single->GetMass() / single->GetMass();
                    a_total = a_total - a;
                }
            }
            single->SetVelocity(single->GetVelocity() + a_total * time_delta);

            // Set the new speed. After collision, there is a slight reduction in speed to simulate the after effect of collision.
            // double pushed_speed = sqrt(single->GetVelocity().SquareMagnitude());
            // if (pushed_speed < single->GetSpeed()) {
            //     single->SetSpeed(pushed_speed + 0.2 * time_delta * (single->GetSpeed() - pushed_speed));
            // }
        }
    }
}