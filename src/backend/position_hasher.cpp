#include "point.h"
#include "position_hasher.h"
#include "utils/math_utils.h"

#include <iostream>
#include <cmath>
#include <utility>

namespace backend {
    namespace {
        // Get all the key that represents the cell in which the line from start to end goes through.
        // This helps us check all the objects that a bullet might go through
        std::vector<size_t> GetBresenhamLine(Vector2 start, Vector2 end, double x_div, double y_div) {
            // Scale the data first
            start.x = start.x / x_div;
            start.y = start.y / y_div;
            end.x = end.x / x_div;
            end.y = end.y / y_div;

            // Now grid cell scales down to 1.0 x 1.0 cell, we can follow the algorithm, as delineated in here:
            // https://gamedev.stackexchange.com/questions/81267/how-do-i-generalise-bresenhams-line-algorithm-to-floating-point-endpoints
            int x = floor(start.x);
            int y = floor(start.y);
            double diff_x = start.x - end.x;
            double diff_y = start.y - end.y;
            bool step_x = diff_x >= 0;
            bool step_y = diff_y >= 0;

            // Array that store all the cells
            std::vector<size_t> cell_keys;
            if (floor(start.x) == floor(end.x)) {
                for (int j = floor(start.y); j <= floor(end.y); j++) {
                    cell_keys.push_back(key(floor(start.x), j));
                }
                return cell_keys;
            } else if (floor(start.y) == floor(end.y)) {
                for (int i = floor(start.x); i <= floor(end.x); i++) {
                    cell_keys.push_back(key(i, floor(start.y)));
                }
                return cell_keys;
            }

            // Straight distance to the first vertical grid boundary
            double x_offset = end.x > start.x ? (ceil(start.x) - start.x) : (start.x - floor(start.x));
            double y_offset = end.y > start.y ? (ceil(start.y) - start.y) : (start.y - floor(start.y));
            double angle = atan2(-diff_y, diff_x);
            double t_max_x = x_offset / cos(angle);
            double t_max_y = y_offset / sin(angle);
            double t_delta_x = 1.0 / cos(angle);
            double t_delta_y = 1.0 / sin(angle);

            // Travel one grid cell at a time, and store the cell key
            int manhattan_distance = abs(floor(end.x) - floor(start.x)) + abs(floor(end.y) - floor(start.y));
            for (int t = 0; t <= manhattan_distance; t++) {
                cell_keys.push_back(key(x, y));
                if (abs(t_max_x) < abs(t_max_y)) {
                    t_max_x += t_delta_x;
                    x += step_x;
                } else {
                    t_max_y += t_delta_y;
                    y += step_y;
                }
            }
            return cell_keys;
        }
    }
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

    const std::vector<Single*> PositionHasher::GetNearbySingles(Vector2 point, double radius) const {
        // Convert x and y position to hash
        int x_hash = point.x / x_div_;
        int y_hash = point.y / y_div_;

        // Calculate look up range based on the radius
        int additional_range_x = radius / x_div_ + 1;
        int additional_range_y = radius / y_div_ + 1;

        // Return all singles given the range
        std::vector<Single*> return_singles;
        for (int i = x_hash - additional_range_x; i <= x_hash + additional_range_x; i++) {
            for (int j = y_hash - additional_range_y; j <= y_hash + additional_range_y; j++) {
                if (singles_map_.find(key(i, j)) == singles_map_.end()) continue;
                for (Single* single : singles_map_.at(key(i, j))) {
                    // Add to return vector only if it is within distance.
                    if (!math_utils::WithinDistance(single->GetPosition(), point, radius)) {
                        continue;
                    }
                    return_singles.push_back(single);
                }
            }
        }
        return return_singles;
    }

    const std::vector<Single*> PositionHasher::GetNearbySingles(Vector2 point) const {
        // Convert x and y position to hash
        int x_hash = point.x / x_div_;
        int y_hash = point.y / y_div_;

        // Return all singles given the range
        std::vector<Single*> return_singles;
        for (int i = x_hash - 1; i <= x_hash + 1; i++) {
            for (int j = y_hash - 1; j <= y_hash + 1; j++) {
                if (singles_map_.find(key(i, j)) == singles_map_.end()) continue;
                for (Single* single : singles_map_.at(key(i, j))) {
                    return_singles.push_back(single);
                }
            }
        }
        return return_singles;
    }

    const std::vector<Single*> PositionHasher::GetSinglesByLine(Vector2 start, Vector2 end) const {
        std::vector<size_t> keys_list = GetBresenhamLine(start, end, x_div_, y_div_);
        std::vector<Single*> return_singles;
        for (size_t k : keys_list) {
            if (singles_map_.find(k) == singles_map_.end()) continue;
            for (Single* single : singles_map_.at(k)) {
                return_singles.push_back(single);
            }
        }
        return return_singles;
    }
}