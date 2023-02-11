#pragma once

#include <string>

namespace backend {
    class GameVariables {
        public:
            // Object radii
            double player_radius = 2;

            // Single movements within the player
            double single_distance_within_player = 1.5;
            double pos_randomization = 0.03;
    };

    static GameVariables game_vars;
    
    static void ModifyVariable(std::string variable, double value) {
        if (variable == "player_radius") {
            game_vars.player_radius = value;
        } else if (variable == "single_distance_within_player") {
            game_vars.single_distance_within_player = value;
        } else if (variable == "pos_randomization") {
            game_vars.pos_randomization = value;
        }
    }
}