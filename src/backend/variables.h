#pragma once

#include <string>

namespace backend {

    struct CombatStats {
        double speed = 30;
        double acceleration = 150;
        double rotation_speed = 20;

        double speed_in_combat = 15;
        double shooting_distance = 50;
    };

    struct GameVariables {
        // Player variables
        double player_radius = 2;
        double player_starting_mass = 800;
        double player_eating_mass_ratio = 0.5;

        // Single variables within the player
        double single_radius = 1.58;
        double single_starting_mass = 400;
        double single_distance_within_player = 1.5;
        double single_pos_randomization = 0.03;
        double single_decision_delay = 0.2;
        double single_decision_delay_variation = 0.15;
        double single_hit_delay = 0.2;
        double single_reload_delay = 0.5;
        double single_reload_delay_variation = 0.15;
        double single_standing_dist = 0.1;
        double single_out_of_reach_coef = 5;
        double single_out_of_reach_speed_mult = 1.4;

        // Unit variables
        double unit_standing_dist = 0.1;
    };

    struct PhysicsStats {
        // Spring push
        double spring_push = 1;
    };

    struct HitscanStats {
        // Bullet range
        double min_range = 0;
        double max_range = 200;
        double bullet_push = 8;
        double bullet_damage = 20;
    };

    static GameVariables g_game_vars;
    static CombatStats g_combat_stats;
    static PhysicsStats g_physics_stats;
    static HitscanStats g_hitscan_stats;
    
    static void ModifyVariable(std::string variable, double value) {
        if (variable == "player_radius") {
            g_game_vars.player_radius = value;
        } else if (variable == "player_starting_mass") {
            g_game_vars.player_starting_mass = value;
        } else if (variable == "player_eating_mass_ratio") {
            g_game_vars.player_eating_mass_ratio = value;
        } else if (variable == "single_radius") {
            g_game_vars.single_radius = value;
        } else if (variable == "single_starting_mass") {
            g_game_vars.single_starting_mass = value;
        } else if (variable == "single_distance_within_player") {
            g_game_vars.single_distance_within_player = value;
        } else if (variable == "single_decision_delay") {
            g_game_vars.single_decision_delay = value;
        } else if (variable == "single_decision_delay_variation") {
            g_game_vars.single_decision_delay_variation = value;
        }  else if (variable == "pos_randomization") {
            g_game_vars.single_pos_randomization = value;
        }
    }
}