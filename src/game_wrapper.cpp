#include <iostream>
#include <memory>

#include "backend/configs.h"
#include "backend/game.h"
#include "backend/player.h"
#include "backend/single/single.h"
#include "game_wrapper.h"

Napi::FunctionReference GameWrapper::constructor;

Napi::Object GameWrapper::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "GameWrapper", {
    InstanceMethod("AddNewPlayer", &GameWrapper::AddNewPlayer),
    InstanceMethod("RemovePlayer", &GameWrapper::RemovePlayer),
    InstanceMethod("Move", &GameWrapper::Move),
    InstanceMethod("Update", &GameWrapper::Update),
    InstanceMethod("GetPlayerPosition", &GameWrapper::GetPlayerPosition),
    InstanceMethod("GetAllSingles", &GameWrapper::GetAllSingles),
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("GameWrapper", func);
  return exports;
}

GameWrapper::GameWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<GameWrapper>(info)  {
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  if (info.Length() != 0) {
    Napi::TypeError::New(env, "No argument expected").ThrowAsJavaScriptException();
  }

  // Create the game config
  backend::GameConfig game_config = {
    /*xl=*/ -1000,
    /*xu=*/ 1000,
    /*yl=*/ -1000,
    /*yu=*/ 1000,
    /*seed=*/ 42,
    /*num_available_singles=*/ 1000,
    /*single_stats=*/ {
        /*speed=*/ 30.0,
        /*acceleration=*/ 150.0,
        /*rotation_speed=*/ 20
    }
  };
  backend::HashingConfig hashing_config = {
    /*x_div=*/ 50.0,
    /*y_div=*/ 50.0
  };
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
        
    return player_or.value()->ChangeGoalPosition(x.DoubleValue(), y.DoubleValue());
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
    backend::Point point_value = player_or.value()->GetPosition();
    point_array[0] = point_value.x;
    point_array[1] = point_value.y;
    return point_array;
}

Napi::Value GameWrapper::GetAllSingles(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function argument GetAllSingles() should not have any argument").ThrowAsJavaScriptException();
    }
    std::vector<backend::Single*> singles = game_instance_->GetAllSingles();
    Napi::Array singles_array = Napi::Array::New(info.Env(), singles.size());
    for (int i = 0; i < singles.size(); i++) {
        backend::Single* single = singles[i];
        
        // Array containing information about each data
        Napi::Array individual_array = Napi::Array::New(info.Env(), 2);

        // Assign ID
        Napi::BigInt64Array id_array = Napi::BigInt64Array::New(info.Env(), 1);
        id_array[0] = (uint64_t) single;
        individual_array[(uint32_t) 0] = id_array;

        // Assign possition values
        backend::Point point_value = single->GetPosition();
        Napi::Float64Array point_array = Napi::Float64Array::New(info.Env(), 2);
        point_array[0] = point_value.x;
        point_array[1] = point_value.y;
        individual_array[1] = point_array;

        // Assign individual to return array
        singles_array[i] = individual_array;
    }
    return singles_array;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return GameWrapper::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)