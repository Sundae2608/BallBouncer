
#include <memory>
#include <optional>

#include "game_controller.h"
#include "environment.h"
#include "player.h"

namespace {
    using backend::Environment;
    using backend::Player;
}

GameController::GameController() {
    environment_ = std::make_unique<Environment>(-1000, 1000, -1000, 1000);
};

GameController::~GameController() {
    environment_.reset();
}

void GameController::Move(std::string player_id, double x, double y) {
    auto player = environment_->GetPlayer(player_id);
    if (player.has_value()) {
        player.value()->ChangeGoalPosition(x, y);
    }
}

void GameController::Update(double time_delta) {
    environment_->Update(time_delta);
}