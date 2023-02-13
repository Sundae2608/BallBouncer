#pragma once

#include <cstdint>

#include "variables.h"

namespace backend {
    struct GameConfig {
        // Map dimension
        double xl;
        double xu;
        double yl;
        double yu;

        // RNG seed
        int seed;

        // Number of random troops
        int num_available_singles;

        // Universal single stats
        SingleStats single_stats;  
    };

    struct HashingConfig {
        double x_div;
        double y_div;
    };
}