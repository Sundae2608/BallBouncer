#include "point.h"
#include "position_hasher.h"
#include "../utils/math_utils.h"

#include <iostream>
#include <utility>

namespace backend {
    PositionHasher::PositionHasher(double x_div, double y_div) :
        x_div_(x_div), y_div_(y_div) { };

    void PositionHasher::RehashObjects() {
        // Clear all elements from previous hash
        for (auto it = singles_map_.begin(); it != singles_map_.end(); it++) {
            it->second.clear();
        }

        // Rehash all objects
        for (auto it = singles_.begin(); it != singles_.end(); it++) {
            Vector2 point = (*it)->GetPosition();
            int x_hash = point.x / x_div_;
            int y_hash = point.y / y_div_;
            singles_map_[key(x_hash, y_hash)].push_back(*it);
        }
    }

    void PositionHasher::AddSingle(Single* single) {
        singles_.insert(single);
    }

    void PositionHasher::RemoveSingle(Single* single) {
        singles_.erase(single);
    }

    const std::vector<Single*> PositionHasher::GetAllSingles() const {
        std::vector<Single*> singles;
        singles.insert(singles.end(), singles_.begin(), singles_.end());
        return singles;
    }

    const std::vector<Single*> PositionHasher::GetNearbySingles(const Vector2 point, double radius) const {
        // Convert x and y position to hash
        int x_hash = point.x / x_div_;
        int y_hash = point.y / y_div_;

        // Calculate look up range based on the radius
        int additional_range_x = radius / x_div_ + 1;
        int additional_range_y = radius / y_div_ + 1;

        // Return all singles given the range
        std::vector<Single*> return_vector;
        for (int i = x_hash - additional_range_x; i <= x_hash + additional_range_x; i++) {
            for (int j = y_hash - additional_range_y; j <= y_hash + additional_range_y; j++) {
                if (singles_map_.find(key(i, j)) == singles_map_.end()) continue;
                for (Single* single : singles_map_.at(key(i, j))) {
                    // Add to return vector only if it is within distance.
                    if (!math_utils::WithinDistance(single->GetPosition(), point, radius)) {
                        continue;
                    }
                    return_vector.push_back(single);
                }
            }
        }
        return return_vector;
    }

    const std::vector<Single*> PositionHasher::GetNearbySingles(const Vector2 point) const {
        // Convert x and y position to hash
        int x_hash = point.x / x_div_;
        int y_hash = point.y / y_div_;

        // Return all singles given the range
        std::vector<Single*> return_vector;
        for (int i = x_hash - 1; i <= x_hash + 1; i++) {
            for (int j = y_hash - 1; j <= y_hash + 1; j++) {
                if (singles_map_.find(key(i, j)) == singles_map_.end()) continue;
                for (Single* single : singles_map_.at(key(i, j))) {
                    return_vector.push_back(single);
                }
            }
        }
        return return_vector;
    }
}