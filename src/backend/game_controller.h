#pragma once

#include <iostream>

#include "environment.h"

class GameController {
    public:
        GameController();
        ~GameController();

        // Move a command from a player with certain ID to position (x, y).
        void Move(std::string player_id, double x, double y);

        // Trigger a command that update the game to the next state.
        void Update(double time_delta);

    private:
        std::unique_ptr<backend::Environment> environment_;
};