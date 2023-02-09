#pragma once

#include <napi.h>
#include "backend/game.h"

class GameWrapper : public Napi::ObjectWrap<GameWrapper> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports); // sInit function for setting the export key to JS
        GameWrapper(const Napi::CallbackInfo& info); // Constructor to initialise

    private:
        static Napi::FunctionReference constructor; //reference to store the class definition that needs to be exported to JS

        // Wrap AddPlayer function
        void AddNewPlayer(const Napi::CallbackInfo& info);

        // Wrap RemovePlayer function
        void RemovePlayer(const Napi::CallbackInfo& info);

        // Wrap Move function
        void Move(const Napi::CallbackInfo& info);

        // Wrap Update function
        void Update(const Napi::CallbackInfo& info);

        // Wrap GetPlayerPosition function
        Napi::Value GetPlayerPosition(const Napi::CallbackInfo& info);
        
        // Get all single positions
        Napi::Value GetSinglePositionsAll(const Napi::CallbackInfo& info);
        Napi::Value GetSinglePositionsByPlayer(const Napi::CallbackInfo& info);
        Napi::Value GetSinglePositionsNeutral(const Napi::CallbackInfo& info);
        
        // Get all Id
        Napi::Value GetSingleIdsAll(const Napi::CallbackInfo& info);
        Napi::Value GetSingleIdsByPlayer(const Napi::CallbackInfo& info);
        Napi::Value GetSingleIdsNeutral(const Napi::CallbackInfo& info);

        // Internally stored game instance
        std::unique_ptr<backend::Game> game_instance_;
};