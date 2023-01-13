// Server code
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
var gameInstance = new game_wrapper.GameWrapper();

var playerCreations = [];
var playerDestructions = [];
var playerMoves = []
var objectPositions = {};

var players = {};

function connected(socket) {
  socket.on('newPlayer', data => {
    console.log("New player coming at id " + socket.id);
    playerCreations.push(socket.id);
  });

  socket.on('disconnect', () => {
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
  }
  playerDestructions = [];
  for (let data of playerMoves) {
    gameInstance.Move(data.playerId, data.x, data.y);
  }
  playerMoves = [];

  // Game update
  currTime = Date.now();
  timeDelta = (currTime - lastTime) / 1000;
  gameInstance.Update(timeDelta);
  for (let playerId in players) {
    let point_array = gameInstance.GetPlayerPosition(playerId);
    objectPositions[playerId] = { x: point_array[0], y: point_array[1]};
  }
  io.emit('updatePlayers', objectPositions);
  lastTime = currTime;
}
setInterval(serverLoop, 1000/60);