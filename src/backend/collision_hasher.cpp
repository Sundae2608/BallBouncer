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
        for (const Single* single : singles_) {
            Point point = single->GetPosition();
            int32_t x_hash = point.x / x_div_;
            int32_t y_hash = point.y / y_div_;
            singles_map_[PairHash(x_hash, y_hash)].push_back(single);
        }
    }

    void CollisionHasher::AddObject(const Single* single) {
        singles_.insert(single);
    }

    void CollisionHasher::RemoveObject(const Single* single) {
        singles_.erase(single);
    }
}