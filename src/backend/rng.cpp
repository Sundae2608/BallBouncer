#include "rng.h"

namespace backend {
    RNG::RNG(unsigned int seed) : rng_(seed), uniform_distr_(0.0, 1.0) {};

    double RNG::RandDouble(double lb, double ub) {
        return uniform_distr_(rng_) * (ub - lb) + lb;
    }

    int RNG::RandInt(int lb, int ub) {
        int rand_int = uniform_distr_(rng_) * (ub - lb) + lb;
        if (rand_int == ub) {
            return ub - 1;
        }
        return rand_int;
    }
}