{
  "targets": [
    {
      "target_name": "game_wrapper",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/game_wrapper.cpp",
        "./src/backend/collision_hasher.cpp",
        "./src/backend/game.cpp",
        "./src/backend/player.cpp",
        "./src/backend/rng.cpp",
        "./src/backend/single/single.cpp",
        "./src/utils/math_utils.cpp",
        "./src/utils/move_utils.cpp",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    },
  ]
}