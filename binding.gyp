{ 
  "actions": [
    {
      "action_name": 'generate_proto_file',
      "inputs": [
        '<(INTERMEDIATE_DIR)/src/proto/data.proto',
      ],
      "outputs": ["<(INTERMEDIATE_DIR)/"],
      "action": ['protoc', '-I=<(INTERMEDIATE_DIR)/src/proto/', '<@(_inputs)', '--cpp_out=<@(_outputs)']
    },
  ],
  "targets": [
    {
      "target_name": "game_wrapper",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/game_wrapper.cpp",
        "./src/backend/boids_manager.cpp",
        "./src/backend/collision_manager.cpp",
        "./src/backend/hitscan_manager.cpp",
        "./src/backend/position_hasher.cpp",
        "./src/backend/game.cpp",
        "./src/backend/player.cpp",
        "./src/backend/rng.cpp",
        "./src/backend/single/single.cpp",
        "./src/backend/single/single_formation.cpp",
        "./src/backend/single/unit.cpp",
        "./src/backend/utils/math_utils.cpp",
        "./src/backend/utils/move_utils.cpp",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_CPP_EXCEPTIONS' ],
    },
  ]
}