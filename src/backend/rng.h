#pragma once

#include <random>

namespace backend {
    class RNG {
        public:
            RNG(unsigned int seed);

            double RandDouble(double lb, double ub);

        private:
            std::default_random_engine rng_; 
            std::uniform_real_distribution<double> uniform_distr_;
    };
}  // namespace backend