#pragma once

#include <random>

namespace backend {
    class RNG {
        public:
            RNG(unsigned int seed);

            // Generate a uniform random double [lb, ub)
            double RandDouble(double lb, double ub);

            // Generate a uniform random integer [lb, ub)
            int RandInt(int lb, int ub);

        private:
            std::default_random_engine rng_; 
            std::uniform_real_distribution<double> uniform_distr_;
    };
}  // namespace backend