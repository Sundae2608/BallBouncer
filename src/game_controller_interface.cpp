#include <napi.h>

#include "game_controller_interface.h"

Napi::FunctionReference GameControllerInterface::constructor;

Napi::Object GameControllerInterface::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "GameControllerInterface", {
        InstanceMethod<&GameControllerInterface::Move>("Move", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&GameControllerInterface::Update>("Update", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("GameControllerInterface", func);
    return exports;
}

GameControllerInterface::GameControllerInterface(const Napi::CallbackInfo& info) :
    Napi::ObjectWrap<GameControllerInterface>(info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int length = info.Length();
    if (length != 0) {
        Napi::TypeError::New(env, "No argument needed to initialize GameController").ThrowAsJavaScriptException();
    }

    this->game_controller_ = new GameController();
}

void GameControllerInterface::Move(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 3 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber()) {
        Napi::TypeError::New(env, "Function argument Move(string, double, double) expected").ThrowAsJavaScriptException();
    }
    auto player_id = info[0].As<Napi::String>();
    auto x = info[1].As<Napi::Number>();
    auto y = info[2].As<Napi::Number>();

    this->game_controller_->Move(
        player_id, x.DoubleValue(), y.DoubleValue());
    return;
}

void GameControllerInterface::Update(const Napi::CallbackInfo& info) {
        Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 1 || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Function argument Update(double) expected").ThrowAsJavaScriptException();
    }
    auto time_delta = info[0].As<Napi::Number>();
    this->game_controller_->Update(time_delta.DoubleValue());
    return;
}