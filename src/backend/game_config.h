#pragma once

#include <cstdint>

#include "backend/single/single_stats.h"

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
}