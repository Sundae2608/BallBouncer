#include "rng.h"

namespace backend {
    RNG::RNG(unsigned int seed) : rng_(seed), uniform_distr_(0.0, 1.0) {};

    double RNG::RandDouble(double lb, double ub) {
        return uniform_distr_(rng_) * (ub - lb) + lb;
    }
}