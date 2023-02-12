#pragma once

#include "../point.h"

namespace backend {
    namespace single {
        class Unit {
            public:
                Unit(Vector2 position);

            private:
                Vector2 p_;
        }
    }
}