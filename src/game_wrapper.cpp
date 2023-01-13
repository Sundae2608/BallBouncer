#include <iostream>
#include <memory>

#include "backend/game.h"
#include "backend/player.h"
#include "backend/game_config.h"
#include "game_wrapper.h"

Napi::FunctionReference ControllerWrapper::constructor;

Napi::Object ControllerWrapper::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "GameWrapper", {
    InstanceMethod("AddNewPlayer", &ControllerWrapper::AddNewPlayer),
    InstanceMethod("RemovePlayer", &ControllerWrapper::RemovePlayer),
    InstanceMethod("Move", &ControllerWrapper::Move),
    InstanceMethod("Update", &ControllerWrapper::Update),
    InstanceMethod("GetPlayerPosition", &ControllerWrapper::GetPlayerPosition),
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("GameWrapper", func);
  return exports;
}

ControllerWrapper::ControllerWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<ControllerWrapper>(info)  {
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
  game_instance_ = std::make_unique<backend::Game>(game_config);
}

void ControllerWrapper::AddNewPlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function argument AddNewPlayer(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    game_instance_->AddNewPlayer(player_id);
    return;
}

void ControllerWrapper::RemovePlayer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function argument RemovePlayer(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();

    game_instance_->RemovePlayer(player_id);
    return;
}

void ControllerWrapper::Move(const Napi::CallbackInfo& info) {
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
        Napi::TypeError::New(env, "Player Id not found").ThrowAsJavaScriptException();
    }
        
    return player_or.value()->ChangeGoalPosition(x.DoubleValue(), y.DoubleValue());
}

void ControllerWrapper::Update(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Function argument Update(double time_delta) expected").ThrowAsJavaScriptException();
    }
    auto time_delta = info[0].As<Napi::Number>();
    game_instance_->Update(time_delta.DoubleValue());
    return;
}

Napi::Value ControllerWrapper::GetPlayerPosition(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function argument GetPlayerPosition(string player_id) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto player_or = game_instance_->GetPlayer(player_id);
    if (!player_or.has_value()) {
        Napi::TypeError::New(env, "Function argument Move(string player_id, double x, double y) expected").ThrowAsJavaScriptException();
    }
    Napi::Float64Array point_array = Napi::Float64Array::New(info.Env(), 2);
    backend::Point point_value = player_or.value()->GetPosition();
    point_array[0] = point_value.x;
    point_array[1] = point_value.y;
    return point_array;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return ControllerWrapper::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)