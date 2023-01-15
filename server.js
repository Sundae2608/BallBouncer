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
var gameInstance = new game_wrapper.GameWrapper(
    /*xl=*/ -200,
    /*xu=*/ 200,
    /*yl=*/ -200,
    /*yu=*/ 200,
    /*seed=*/ 42,
    /*num_available_singles=*/ 500
);

var playerCreations = [];
var playerDestructions = [];
var playerMoves = [];

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
    delete players[playerId];
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
  let data = {};

  // Create data to send for each player
  let playerData = {};
  for (let playerId in players) {
    let pointArray = gameInstance.GetPlayerPosition(playerId);
    playerData[playerId] = {
      playerPosition: { x: pointArray[0], y: pointArray[1] },
    };
  }

  // Get the data for each position
  let singlesData = {
    singlesPosition: gameInstance.GetAllSinglesPosition(),
    singlesId: gameInstance.GetAllSinglesId()
  }

  // Construct the overall data
  data = {
    playerData: playerData,
    singlesData: singlesData
  }
  io.emit('updatePlayers', data);
  lastTime = currTime;
}
setInterval(serverLoop, 1000/60);