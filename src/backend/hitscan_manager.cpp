#include <float.h>
#include <iostream>

#include "hitscan_manager.h"
#include "point.h"
#include "position_hasher.h"
#include "variables.h"
#include "single/single.h"
#include "utils/math_utils.h"

namespace backend {
    HitscanManager::HitscanManager(const PositionHasher& position_hasher) : position_hasher_(position_hasher) {};

    void HitscanManager::AddObject(HitscanObject& hitscan_object) {
        hitscan_objects_.push_back(hitscan_object);
    }

    void HitscanManager::ProcessCollision() {
        for (HitscanObject& hitscan_object : hitscan_objects_) {
            Vector2 start = hitscan_object.p_source + math_utils::GetCartesianVector(g_hitscan_stats.min_range, hitscan_object.angle);
            Vector2 end = hitscan_object.p_source + math_utils::GetCartesianVector(g_hitscan_stats.max_range, hitscan_object.angle);
            // std::vector<Single*> singles = position_hasher_.GetSinglesByLine(start, end);
            std::vector<Single*> singles = position_hasher_.GetAllSingles();

            double min_sq_dist = DBL_MAX;
            Single* hit_single = nullptr;
            for (Single* single : singles) {
                if (math_utils::LineCircleIntersection(start, end, single->GetPosition(), single->GetRadius()) && single->GetFactionId() != hitscan_object.faction_id) {
                    double sq_dist = (single->GetPosition() - hitscan_object.p_source).SquareMagnitude();
                    if (sq_dist < min_sq_dist) {
                        hit_single = single;
                        min_sq_dist = sq_dist;
                    }
                }
            }
            if (hit_single != nullptr) {
                hit_single->GetHit(-hitscan_object.angle, g_hitscan_stats.bullet_damage);
            }
        }

        // Clear the hitscan object
        hitscan_objects_.clear();
    }
}