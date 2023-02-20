#include <iostream>
#include <memory>

#include "backend/configs.h"
#include "backend/game.h"
#include "backend/player.h"
#include "backend/single/single.h"
#include "backend/variables.h"
#include "game_wrapper.h"

namespace {
    Napi::Array CreatePointArrayFromSingles(std::vector<backend::Single*> singles, const Napi::CallbackInfo& info) {
        Napi::Array singles_array = Napi::Array::New(info.Env(), singles.size());
        for (int i = 0; i < singles.size(); i++) {
            backend::Single* single = singles[i];
            
            // Assign possition values
            backend::Vector2 point_value = single->GetPosition();
            Napi::Float64Array point_array = Napi::Float64Array::New(info.Env(), 2);
            point_array[0] = point_value.x;
            point_array[1] = point_value.y;

            // Assign individual to return array
            singles_array[i] = point_array;
        }
        return singles_array;
    }

    Napi::Int32Array CreateIdFromSingles(std::vector<backend::Single*> singles, const Napi::CallbackInfo& info) {
        Napi::Int32Array ids_array = Napi::Int32Array::New(info.Env(), singles.size());
        for (int i = 0; i < singles.size(); i++) {
            backend::Single* single = singles[i];
            
            // Assign individual ID to return array
            ids_array[i] = single->GetId();
        }
        return ids_array;
    }

    Napi::Float64Array CreateHitDelaysFromSingles(std::vector<backend::Single*> singles, const Napi::CallbackInfo& info) {
        Napi::Float64Array hit_delays_array = Napi::Float64Array::New(info.Env(), singles.size());
        for (int i = 0; i < singles.size(); i++) {
            backend::Single* single = singles[i];
            
            // Assign individual ID to return array
            hit_delays_array[i] = single->GetHitDelay();
        }
        return hit_delays_array;
    }

    Napi::Int8Array CreateJustShootsFromSingles(std::vector<backend::Single*> singles, const Napi::CallbackInfo& info) {
        Napi::Int8Array just_shoots_array = Napi::Int8Array::New(info.Env(), singles.size());
        for (int i = 0; i < singles.size(); i++) {
            backend::Single* single = singles[i];
            
            // Assign individual ID to return array
            just_shoots_array[i] = single->JustShoot() ? 1 : 0;
        }
        return just_shoots_array;
    }
}

Napi::FunctionReference GameWrapper::constructor;

Napi::Object GameWrapper::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "GameWrapper", {
    // Add or remove player
    InstanceMethod("AddNewPlayer", &GameWrapper::AddNewPlayer),
    InstanceMethod("RemovePlayer", &GameWrapper::RemovePlayer),

    // Movement control
    InstanceMethod("Move", &GameWrapper::Move),

    // Update
    InstanceMethod("Update", &GameWrapper::Update),

    // Getting data
    InstanceMethod("GetPlayerPosition", &GameWrapper::GetPlayerPosition),
    InstanceMethod("GetPlayerMass", &GameWrapper::GetPlayerRadius),
    InstanceMethod("GetSinglePositionsAll", &GameWrapper::GetSinglePositionsAll),
    InstanceMethod("GetSinglePositionsByPlayer", &GameWrapper::GetSinglePositionsByPlayer),
    InstanceMethod("GetSinglePositionsNeutral", &GameWrapper::GetSinglePositionsNeutral),

    InstanceMethod("GetSingleIdsAll", &GameWrapper::GetSingleIdsAll),
    InstanceMethod("GetSingleIdsByPlayer", &GameWrapper::GetSingleIdsByPlayer),
    InstanceMethod("GetSingleIdsNeutral", &GameWrapper::GetSingleIdsByPlayer),

    InstanceMethod("GetHitDelaysAll", &GameWrapper::GetHitDelaysAll),
    InstanceMethod("GetHitDelaysByPlayer", &GameWrapper::GetHitDelaysByPlayer),
    InstanceMethod("GetHitDelaysNeutral", &GameWrapper::GetHitDelaysNeutral),

    InstanceMethod("GetJustShootsAll", &GameWrapper::GetJustShootsAll),
    InstanceMethod("GetJustShootsByPlayer", &GameWrapper::GetJustShootsByPlayer),
    InstanceMethod("GetJustShootsNeutral", &GameWrapper::GetJustShootsNeutral),

    // Update tuning variables
    InstanceMethod("NodifyVariable", &GameWrapper::ModifyVariable),

    // Player attack player
    InstanceMethod("PlayerAttackPlayer", &GameWrapper::PlayerAttackPlayer),
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("GameWrapper", func);
  return exports;
}

GameWrapper::GameWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<GameWrapper>(info)  {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() != 6 ||
      !info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsNumber() || 
      !info[3].IsNumber() || !info[4].IsNumber() || !info[5].IsNumber()) {
    Napi::TypeError::New(env, 
        "Construct should be initialized with the following argument "
        "GameWrapper(double xl, double xu, double yl, double yu, "
        "int32_t seed, int32_t num_singles)").ThrowAsJavaScriptException();
  }

  // Create the game config
  double xl = info[0].As<Napi::Number>();
  double xu = info[1].As<Napi::Number>();
  double yl = info[2].As<Napi::Number>();
  double yu = info[3].As<Napi::Number>();
  int32_t seed = info[4].As<Napi::Number>();
  int32_t num_singles = info[5].As<Napi::Number>();

  backend::GameConfig game_config;
  game_config.xl = xl;
  game_config.xu = xu;
  game_config.yl = yl;
  game_config.yu = yu;
  game_config.seed = seed;
  game_config.num_available_singles = num_singles;

  game_config.combat_stats = backend::g_combat_stats;

  backend::HashingConfig hashing_config;
  hashing_config.x_div = 50.0;
  hashing_config.y_div = 50.0;
  game_instance_ = std::make_unique<backend::Game>(game_config, hashing_config);
}

void GameWrapper::AddNewPlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function argument AddNewPlayer(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    game_instance_->AddNewPlayer(player_id);
    return;
}

void GameWrapper::RemovePlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function argument RemovePlayer(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();

    game_instance_->RemovePlayer(player_id);
    return;
}

void GameWrapper::Move(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 3 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber()) {
        Napi::TypeError::New(env, "Function argument Move(string player_id, double x, double y) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto x = info[1].As<Napi::Number>();
    auto y = info[2].As<Napi::Number>();

    auto player_or = game_instance_->GetPlayer(player_id);
    if (!player_or.has_value()) {
        Napi::TypeError::New(env, "Player ID not found").ThrowAsJavaScriptException();
    }
        
    return player_or.value()->MoveTo({x.DoubleValue(), y.DoubleValue()});
}

void GameWrapper::Update(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Function argument Update(double time_delta) expected").ThrowAsJavaScriptException();
    }
    auto time_delta = info[0].As<Napi::Number>();
    game_instance_->Update(time_delta.DoubleValue());
    return;
}

Napi::Value GameWrapper::GetPlayerPosition(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function argument GetPlayerPosition(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto player_or = game_instance_->GetPlayer(player_id);
    if (!player_or.has_value()) {
        Napi::TypeError::New(env, "Player ID not found").ThrowAsJavaScriptException();
    }
    Napi::Float64Array point_array = Napi::Float64Array::New(info.Env(), 2);
    backend::Vector2 point_value = player_or.value()->GetPosition();
    point_array[0] = point_value.x;
    point_array[1] = point_value.y;
    return point_array;
}

Napi::Value GameWrapper::GetPlayerRadius(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function argument GetPlayerRadius(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto player_or = game_instance_->GetPlayer(player_id);
    if (!player_or.has_value()) {
        Napi::TypeError::New(env, "Player ID not found").ThrowAsJavaScriptException();
    }
    return Napi::Number::New(env, player_or.value()->GetSingle()->GetRadius());
}

Napi::Value GameWrapper::GetSinglePositionsAll(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetAllSinglesPosition() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetAllSingles();
    return CreatePointArrayFromSingles(singles, info);
}

Napi::Value GameWrapper::GetSinglePositionsByPlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 1 || info[1].IsString()) {
        Napi::TypeError::New(env, "Function argument GetSinglePositionsByPlayer(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto singles_or = game_instance_->GetPlayerSingles(player_id);
    if (!singles_or.has_value()) {
        Napi::TypeError::New(env, "Player ID not found").ThrowAsJavaScriptException();
    }
    return CreatePointArrayFromSingles(singles_or.value(), info);
}

Napi::Value GameWrapper::GetSinglePositionsNeutral(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetSinglePositionsNeutral() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetNeutralSingles();
    return CreatePointArrayFromSingles(singles, info);
}

Napi::Value GameWrapper::GetSingleIdsAll(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetAllSingles() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetAllSingles();
    return CreateIdFromSingles(singles, info);
}

Napi::Value GameWrapper::GetSingleIdsByPlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 1 || info[1].IsString()) {
        Napi::TypeError::New(env, "Function argument GetSingleIdsByPlayer(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto singles_or = game_instance_->GetPlayerSingles(player_id);
    if (!singles_or.has_value()) {
        Napi::TypeError::New(env, "Player ID not found").ThrowAsJavaScriptException();
    }
    return CreateIdFromSingles(singles_or.value(), info);
}

Napi::Value GameWrapper::GetSingleIdsNeutral(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetSingleIdsNeutral() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetNeutralSingles();
    return CreateIdFromSingles(singles, info);
}

Napi::Value GameWrapper::GetHitDelaysAll(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetHitDelaysAll() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetAllSingles();
    return CreateHitDelaysFromSingles(singles, info);
}

Napi::Value GameWrapper::GetHitDelaysByPlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 1 || info[1].IsString()) {
        Napi::TypeError::New(env, "Function argument GetHitDelaysByPlayer(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto singles_or = game_instance_->GetPlayerSingles(player_id);
    if (!singles_or.has_value()) {
        Napi::TypeError::New(env, "Player ID not found").ThrowAsJavaScriptException();
    }
    return CreateHitDelaysFromSingles(singles_or.value(), info);
}

Napi::Value GameWrapper::GetHitDelaysNeutral(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetHitDelaysNeutral() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetNeutralSingles();
    return CreateHitDelaysFromSingles(singles, info);
}

Napi::Value GameWrapper::GetJustShootsAll(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetJustShootsAll() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetAllSingles();
    return CreateJustShootsFromSingles(singles, info);
}

Napi::Value GameWrapper::GetJustShootsByPlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 1 || info[1].IsString()) {
        Napi::TypeError::New(env, "Function argument GetJustShootsByPlayer(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto singles_or = game_instance_->GetPlayerSingles(player_id);
    if (!singles_or.has_value()) {
        Napi::TypeError::New(env, "Player ID not found").ThrowAsJavaScriptException();
    }
    return CreateJustShootsFromSingles(singles_or.value(), info);
}

Napi::Value GameWrapper::GetJustShootsNeutral(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetJustShootsNeutral() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetNeutralSingles();
    return CreateJustShootsFromSingles(singles, info);
}


void GameWrapper::ModifyVariable(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 2 && !info[0].IsString() && !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Function argument ModifyVariable(string variable, double value) expected").ThrowAsJavaScriptException();
    }
    auto variable = info[0].As<Napi::String>();
    auto value = info[1].As<Napi::Number>();
    backend::ModifyVariable(variable, value);
}

void GameWrapper::PlayerAttackPlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 2 && !info[0].IsString() && !info[1].IsString()) {
        Napi::TypeError::New(env, "Function argument PlayerAttackPlayer(string player1_id, string player2_id) expected").ThrowAsJavaScriptException();
    }
    auto player1_id = info[0].As<Napi::String>();
    auto player2_id = info[1].As<Napi::String>();
    auto player1_or = game_instance_->GetPlayer(player1_id);
    auto player2_or = game_instance_->GetPlayer(player2_id);
    if (!player1_or.has_value()) {
        Napi::TypeError::New(env, "Player ID 1 not found").ThrowAsJavaScriptException();
    }
    if (!player2_or.has_value()) {
        Napi::TypeError::New(env, "Player ID 2 not found").ThrowAsJavaScriptException();
    }
    player1_or.value()->AttackPlayer(player2_or.value());
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return GameWrapper::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)