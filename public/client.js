const socket = io.connect();
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

let clientBalls = {};
let selfID;

putWallsAround(0, 0, canvas.clientWidth, canvas.clientHeight);

socket.on('connect', () => {
    selfID = socket.id;
    let startX = 40 + Math.random() * 560;
    let startY = 40 + Math.random() * 400;
    clientBalls[socket.id] = new Ball(startX, startY, 40, 5);
    clientBalls[socket.id].player = true;
    clientBalls[socket.id].maxSpeed = 5;
    userInput(clientBalls[socket.id]);
    socket.emit('newPlayer', {x : startX, y: startY});
})

socket.on('updatePlayers', players => {
    ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
    playersFound = {};
    for(let id in players){
        if(clientBalls[id] === undefined && id !== socket.id){
            clientBalls[id] = new Ball(players[id].x, players[id].y, 40, 5);
            clientBalls[id].maxSpeed = 5;
        }
        playersFound[id] = true;
    }
    for(let id in clientBalls){
        if(!playersFound[id]){
            clientBalls[id].remove();
            delete clientBalls[id];
        }
    }
})

socket.on('updatePositions', playerPositions => {
    for(let id in playerPositions){
        if(clientBalls[id] !== undefined){
            clientBalls[id].setPosition(playerPositions[id].x, playerPositions[id].y, playerPositions[id].angle);
        }
    }
})

function gameLogic() {
    socket.emit('update', {x: playerBall.pos.x, y:playerBall.pos.y});
}

requestAnimationFrame(renderOnly);