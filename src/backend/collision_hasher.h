#pragma once

#include <stdint.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "single/single.h"

namespace backend {
    class CollisionHasher {
        public:
            // Construct for the collision hasher. The hasher takes x_div and y_div, which represents
            // a unit of space in which the object will be hashed into.
            CollisionHasher(double x_div, double y_div);

            // Add a new object into the hasher
            void AddSingle(Single* single);

            // Remove an object from the hasher
            void RemoveSingle(Single* single);

            // Rehash all the objects given the current position of each single.
            void RehashObjects();

            // Get nearby singles given a radius
            std::vector<Single*> GetNearbySingles(double x, double y, double radius);
            std::vector<Single*> GetNearbySingles(const Point& point, double radius);
        private:
            double x_div_;
            double y_div_;
            std::unordered_set<Single*> singles_;
            std::unordered_map<int64_t, std::vector<Single*>> singles_map_;
    };
}