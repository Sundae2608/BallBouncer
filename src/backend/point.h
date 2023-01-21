#pragma once

namespace backend {
    struct Vector2 {
        double x;
        double y;

        Vector2 operator+(const Vector2 v) {
            Vector2 result;
            result.x = this->x + v.x;
            result.y = this->y + v.y;
            return result;
        }

        Vector2 operator-(const Vector2 v) {
            Vector2 result;
            result.x = this->x - v.x;
            result.y = this->y - v.y;
            return result;
        }

        Vector2 operator/(double denominator) {
            Vector2 result;
            result.x = this->x / denominator;
            result.y = this->y / denominator;
            return result;
        }
    };
}