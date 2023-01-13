#pragma once

#include <stdint.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "backend/single/single.h"

namespace backend {
    class CollisionHasher {
        public:
            CollisionHasher(double x_div, double y_div);
            void AddObject(const Single* single);
            void RemoveObject(const Single* single);
            void RehashObjects();
        private:
            int64_t PairHash(int32_t x_hash, int32_t y_hash);

            double x_div_;
            double y_div_;
            std::unordered_set<const Single*> singles_;
            std::unordered_map<int64_t, std::vector<const Single*>> singles_map_;
    };
}