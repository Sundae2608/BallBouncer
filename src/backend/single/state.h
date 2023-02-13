#pragma once

namespace backend {
    enum SingleState {
        SINGLE_STANDING = 0,
        SINGLE_MOVING = 1,
        SINGLE_ROTATING = 2,
        SINGLE_ENGAGING = 3,
        SINGLE_DEAD = 4,
    };

    enum UnitState {
        UNIT_STANDING = 0,
        UNIT_MOVING = 1,
        UNIT_ENGAGING = 2
    };
}