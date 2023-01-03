{
  "targets": [
    {
      "target_name": "game_controller_interface",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/game_controller_interface.cpp",
        "./src/backend/environment.cpp",
        "./src/backend/game_controller.cpp",
        "./src/backend/player.cpp",
        "./src/backend/rng.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}