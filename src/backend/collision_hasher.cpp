#include "point.h"
#include "collision_hasher.h"

#include <iostream>

namespace backend {
    namespace {
        int64_t PairHash(int32_t x_hash, int32_t y_hash) {
                return ((int64_t) x_hash << 32) | (y_hash & 0XFFFFFFFFL);
        }
    }

    CollisionHasher::CollisionHasher(double x_div, double y_div) :
        x_div_(x_div), y_div_(y_div) {};

    void CollisionHasher::RehashObjects() {
        singles_map_.clear();
        for (Single* single : singles_) {
            Point point = single->GetPosition();
            int32_t x_hash = point.x / x_div_;
            int32_t y_hash = point.y / y_div_;
            singles_map_[PairHash(x_hash, y_hash)].push_back(single);
        }
    }

    void CollisionHasher::AddSingle(Single* single) {
        singles_.insert(single);
    }

    void CollisionHasher::RemoveSingle(Single* single) {
        singles_.erase(single);
    }

    std::vector<Single*> CollisionHasher::GetNearbySingles(double x, double y, double radius) {
        // Convert x and y position to hash
        int32_t x_hash = x / x_div_;
        int32_t y_hash = y / y_div_;

        // Calculate look up range based on the radius
        int32_t additional_range_x = radius / x_div_ + 1;
        int32_t additional_range_y = radius / y_div_ + 1;

        // Return all singles given the range
        std::vector<Single*> return_vector;
        for (int i = x_hash - additional_range_x; i <= x_hash + additional_range_x; i++) {
            for (int j = y_hash - additional_range_y; j <= y_hash + additional_range_y; j++) {
                int64_t hash_key = PairHash(x_hash, y_hash);
                for (Single* single : singles_map_[hash_key]) {
                    return_vector.push_back(single);
                }
            }
        }
        return return_vector;
    }

    std::vector<Single*> CollisionHasher::GetNearbySingles(const Point& point, double radius) {
        return GetNearbySingles(point.x, point.y, radius);
    }
}