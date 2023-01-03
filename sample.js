const game_backend = require('./build/Release/game.node');

// See properities on 'exports' object
const gameController = game_backend.GameControllerInterface();
console.log('exports: ', gameController);
console.log();