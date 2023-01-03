#include <napi.h>
#include "backend/game_controller.h"

class GameControllerInterface : public Napi::ObjectWrap<GameControllerInterface> {
    public:
        // Init function for setting the export key to JS
        static Napi::Object Init(Napi::Env env, Napi::Object exports);

        // Construct to iniialize
        GameControllerInterface(const Napi::CallbackInfo& info);

    private:
        // Reference to store the class definition that needs to be exported to JS
        static Napi::FunctionReference constructor;

        // Wrap Move function
        void Move(const Napi::CallbackInfo& info);

        // Wrap Update function
        void Update(const Napi::CallbackInfo& info);

        // The actual game controller class
        GameController* game_controller_;
};