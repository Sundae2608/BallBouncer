// An abstract player class, just for demo purpose
class Player {
    constructor(playerId, x, y) {
        this.playerId = playerId
        this.x = x
        this.y = y
    }
}

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

let players = {};
let playerPositions = {};

function connected(socket) {
  socket.on('newPlayer', data => {
    console.log("New client connected with id: " + socket.id);
    players[socket.id] = new Player(socket.id, (Math.random() - 1.0) * 20, (Math.random() -1.0) * 20);
    playerPositions[socket.id] = { 
        x: players[socket.id].x,
        y: players[socket.id].y
    };
    console.log("Current number of clients: " + Object.keys(players).length);
    io.emit('updatePlayers', playerPositions);
  });

  socket.on('disconnect', () => {
    delete players[socket.id];
    delete playerPositions[socket.id];
    console.log("Client with id " + socket.id + " left");
    io.emit('updatePlayers', playerPositions);
  });
  
  socket.on('move', moveData => {
    console.log("User with ID: x=" + moveData.x + " y=" + moveData.y);
  });
}

// Server loop
function serverLoop() {
  io.emit('updatePlayers', playerPositions);
}
setInterval(serverLoop, 1000/60);