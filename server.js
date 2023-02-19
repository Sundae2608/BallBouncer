// Server code
process.on('uncaughtException', function (exception) {
  console.log(exception); // to see your exception details in the console
  // if you are on production, maybe you can send the exception details to your
  // email as well ?
});

const express = require('express')
const app = express()
const PORT = process.env.PORT || 3000;
app.use(express.static('public'));

const http = require('http').Server(app);
const io = require("socket.io")(http, {
  cors: {
    origin: "*",
  }
});

app.get('/', (req, res) => res.sendFile(__dirname + '/index.html'));
http.listen(PORT, function() {
    console.log(`Listening on ${PORT}`)
})

io.on('connection', connected);

// Backend initialization
const game_wrapper = require('./build/Release/game_wrapper.node')
var gameInstance = new game_wrapper.GameWrapper(
    /*xl=*/ -200,
    /*xu=*/ 200,
    /*yl=*/ -200,
    /*yu=*/ 200,
    /*seed=*/ 42,
    /*num_available_singles=*/ 300
);

var playerCreations = [];
var playerDestructions = [];
var playerMoves = [];
var playerAttacks = [];

var players = {};

function connected(socket) {
  socket.on('newPlayer', data => {
    console.log("New player coming at id " + socket.id);
    playerCreations.push(socket.id);
  });

  socket.on('disconnect', () => {
    console.log("Player at id " + socket.id + " disconnected");
    playerDestructions.push(socket.id);
  });
  
  socket.on('move', moveData => {
    playerMoves.push(
      {
        playerId: socket.id,
        x: moveData.x,
        y: moveData.y
      }
    );
  });
  
  socket.on('attack', playerId => {
    playerAttacks.push(
      {
        playerId1: socket.id,
        playerId2: playerId
      }
    );
  });

  socket.on('modifyVariable', data => {
    gameInstance.ModifyVariable(data.variable, data.value);
  });
}

// Server loop
var lastTime = Date.now();
function serverLoop() {
  // Player update
  for (let playerId of playerCreations) {
    gameInstance.AddNewPlayer(playerId);
    players[playerId] = true;
  }
  playerCreations = [];
  for (let playerId of playerDestructions) {
    gameInstance.RemovePlayer(playerId);
    delete players[playerId];
  }
  playerDestructions = [];
  for (let data of playerMoves) {
    gameInstance.Move(data.playerId, data.x, data.y);
  }
  playerMoves = [];
  for (let data of playerAttacks) {
    gameInstance.PlayerAttackPlayer(data.playerId1, data.playerId2);
  }
  playerAttacks = [];

  // Game update
  currTime = Date.now();
  timeDelta = (currTime - lastTime) / 1000;
  gameInstance.Update(timeDelta); 

  // Create data to send for each player
  let playerData = {};
  for (let playerId in players) {
    let pointArray = gameInstance.GetPlayerPosition(playerId);
    playerData[playerId] = {
      position: gameInstance.GetPlayerPosition(playerId),
      radius: gameInstance.GetPlayerMass(playerId),
      singlePositions: gameInstance.GetSinglePositionsByPlayer(playerId),
      singleIds: gameInstance.GetSingleIdsByPlayer(playerId),
    };
  }

  // Get the data for each position
  let neutralSingles = {
    singlePositions: gameInstance.GetSinglePositionsAll(),
    singleIds: gameInstance.GetSingleIdsAll()
  }

  // Construct the overall data
  let data = {
    playerData: playerData,
    neutralSingles: neutralSingles
  }

  for (let playerId in players) {
    io.to(playerId).emit('updateData', data);
  }
  lastTime = currTime;
}
setInterval(serverLoop, 1000/60);