import * as THREE from 'three'
import { OrbitControls } from 'https://unpkg.com/three@0.138.3/examples/jsm/controls/OrbitControls.js'
import { TransformControls } from 'https://unpkg.com/three@0.138.3/examples/jsm/controls/TransformControls.js'
import { GUI } from 'https://unpkg.com/three@0.138.3/examples/jsm/libs/lil-gui.module.min.js'
import Stats from 'https://unpkg.com/three@0.138.3/examples/jsm/libs/stats.module'

const randomColor = (() => {
    "use strict";
  
    const randomInt = (min, max) => {
      return Math.floor(Math.random() * (max - min + 1)) + min;
    };
  
    return () => {
      var h = randomInt(0, 360);
      var s = randomInt(42, 60);
      var l = randomInt(40, 40);
      return `hsl(${h},${s}%,${l}%)`;
    };
  })();


// Object set up
let selfID;
let playerObjects = {};
let singleObjects = {};
let clickableSingles = [];
let clickableSinglesToPlayerId = {};
let playerColorMap = {};

function processPlayerData(playerData, playerObjects, playersFound, playerColorMap, scene) {
    for (let playerId in playerData) {
        playersFound[playerId] = true;
        let pos = new Float64Array(playerData[playerId].position);
        let radius = playerData[playerId].radius;
        if (!(playerId in playerObjects)) {
            // Pick a color for the player
            const color = randomColor();
            playerColorMap[playerId] = color;

            // Create an object with that color
            const geometry = new THREE.SphereGeometry( 2, 40, 40 );
            const material = new THREE.MeshToonMaterial( {color: color} );
            const obj = new THREE.Mesh( geometry, material );
            obj.castShadow = true;
            scene.add(obj);
            playerObjects[playerId] = obj;
            clickableSingles.push(obj)
        }
        let playerObject = playerObjects[playerId];
        clickableSinglesToPlayerId[playerObject.uuid] = playerId;
        playerObject.scale.setScalar(radius / 2);
        playerObject.position.x = pos[0];
        playerObject.position.y = pos[1];
        playerObject.position.z = radius / 2;
    }
}

function processSingleData(singlesData, singleObjects, singlesFound, playerColorMap, playerId, scene) {
    let color;
    if (playerId == null) {
        color = 0x696969;
    } else {
        color = playerColorMap[playerId];
    }
    let singlePositions = singlesData.singlePositions;
    let singleIds = new Int32Array(singlesData.singleIds);
    for (let i = 0; i < singleIds.length; i++) {
        let singleId = singleIds[i];
        let singlePosition = new Float64Array(singlePositions[i]);
        singlesFound[singleId] = true;

        if (!(singleId in singleObjects)) {
            const geometry = new THREE.SphereGeometry( 1.58, 40, 40 );
            const material = new THREE.MeshToonMaterial( {color: 0x696969} );
            const obj = new THREE.Mesh( geometry, material );
            obj.castShadow = true;
            scene.add(obj);
            singleObjects[singleId] = obj;
            clickableSingles.push(obj)
        }
        let singleObject = singleObjects[singleId];
        clickableSinglesToPlayerId[singleObject.uuid] = playerId;
        singleObject.position.x = singlePosition[0];
        singleObject.position.y = singlePosition[1];
        singleObject.position.z = 1.58 / 2;
        singleObject.material.color.set(color);
    }
}

// Set up variable
const CAMERA_CONTROL = false;

// Scene and renderer setup
const scene = new THREE.Scene();
const renderer = new THREE.WebGLRenderer( { antialias: true } );
renderer.shadowMap.enabled = true;
renderer.setSize( window.innerWidth, window.innerHeight );
document.body.appendChild( renderer.domElement );

// GUI
const gameVariables = {
    playerRadius: 1.5
}
const gui = new GUI();
let folderLocal = gui.addFolder( 'Game variables' );
let propsLocal = {

    get 'player_radius'() {
        return gameVariables.playerRadius;
    },
    set 'player_radius'(v) {
        gameVariables.playerRadius = v;
        data = {
            variable: "player_radius",
            value: v
        }
        socket.emit('modifyVariable', data);
    },
};
folderLocal.add( propsLocal, 'player_radius', 1.0, 4.0 );

// Camera set up
const CAMERA_BASE_DISTANCE = 600;
const CAMERA_BASE_ANGLE = 0.6;
const CAMERA_ZOOM_TIME = 0.3;
const CAMERA_ZOOM_COEFFICIENT = 1.1;

var cameraDistance = CAMERA_BASE_DISTANCE;
var cameraTowardDistance = cameraDistance;
var cameraRemainingZoomTime = 0;
var cameraOffsetX = 0;
var cameraOffsetY = -Math.cos(CAMERA_BASE_ANGLE) * cameraDistance;
var cameraOffsetZ = Math.sin(CAMERA_BASE_ANGLE) * cameraDistance;
var camera = new THREE.PerspectiveCamera( 10, window.innerWidth / window.innerHeight, 1, 10000 );
camera.position.set( 0, cameraOffsetY, cameraOffsetZ );
camera.lookAt(new THREE.Vector3(0, 0, 0));

// Controls
if (CAMERA_CONTROL) {
    const orbitalControl = new OrbitControls( camera, renderer.domElement );
    orbitalControl.damping = 0.2;
    
    const transformControl = new TransformControls( camera, renderer.domElement );
    scene.add( transformControl );
}

// Audio set up
const listener = new THREE.AudioListener();
camera.add(listener);
const leftClickSound = new THREE.Audio(listener);
const rightClickSound = new THREE.Audio(listener);

// Load a sound and set it to buffer
const audioLoader = new THREE.AudioLoader();
audioLoader.load('audio/click1.mp3', function(buffer) {
    rightClickSound.setBuffer(buffer);
    rightClickSound.setVolume(0.25);
})
audioLoader.load('audio/click2.mp3', function(buffer) {
    leftClickSound.setBuffer(buffer);
    leftClickSound.setVolume(0.25);
})

// Light setup
// scene.background = new THREE.Color( 0xf0f0f0 );		
scene.background = new THREE.Color( 0xf0f0f0 );		
scene.add( new THREE.AmbientLight( 0xf0f0f0 , 0.7) );		
const directionalLight = new THREE.DirectionalLight( 0xffffff, 1.0 );
directionalLight.position.set( 1, -1, 1 );
directionalLight.castShadow = true;
scene.add( directionalLight );

// Box setup
const boxGeometry = new THREE.BoxGeometry( 2000, 2000, 0.1 );
const boxMaterial = new THREE.MeshBasicMaterial( { color: 0xf0f0f0, opacity: 1.0 } );
const box = new THREE.Mesh( boxGeometry, boxMaterial );
box.position.x = 0;
box.position.y = 0;
box.position.z = -0.1;
scene.add( box );

// Box setup
const shadowGeometry = new THREE.BoxGeometry( 2000, 2000, 0.1 );
const shadownMaterial = new THREE.ShadowMaterial( { opacity: 1.0 } );
const shadowBox = new THREE.Mesh( boxGeometry, shadownMaterial );
shadowBox.receiveShadow = true;
shadowBox.position.x = 0;
shadowBox.position.y = 0;
shadowBox.position.z = -0.2;
scene.add( shadowBox );

// Helper setup
const helper = new THREE.GridHelper( 2000, 200 );
helper.position.z = 0;
helper.material.opacity = 0.25;
helper.material.transparent = true;
helper.rotateX( - Math.PI / 2 );
helper.rotateY( Math.PI / 4);
scene.add( helper );

// Raycaster setup
const raycaster = new THREE.Raycaster();

const casterMarkerGeometry = new THREE.SphereGeometry( 0.4, 40, 40 );
const casterMarkerMaterial = new THREE.MeshBasicMaterial( { color: 0x000000, opacity: 1.0 } );
const casterMarker = new THREE.Mesh( casterMarkerGeometry, casterMarkerMaterial );
scene.add(casterMarker);

casterMarker.position.x = 0;
casterMarker.position.y = 0;
casterMarker.position.z = 0;

var movePosition = null;
const pointer = new THREE.Vector2();
function onPointerMove(event) {
	pointer.x = ( event.clientX / window.innerWidth ) * 2 - 1;
	pointer.y = - ( event.clientY / window.innerHeight ) * 2 + 1;
}
window.addEventListener('pointermove', onPointerMove)

function onPointerDown(event) {
    // Move if it is a right click
    if (event.button == 2) {
        rightClickSound.play();
        if (movePosition !== null) {
            socket.emit('move', movePosition);
        }
    } else if (event.button == 0) {
        leftClickSound.play();
    }
}
window.addEventListener('mouseup', onPointerDown)
window.addEventListener('contextmenu', event => {event.preventDefault()});

// Stats setup
const stats = Stats()
document.body.append(stats.domElement)

// Socket setup
const socket = io.connect('http://localhost:3000');

socket.on('connect', () => {
    selfID = socket.id;
    console.log("We have been connected.");
    socket.emit('newPlayer');
});

socket.on('updateData', data => {
    // Loop through all player objects
    let playersFound = {};
    let playerData = data.playerData;
    processPlayerData(playerData, playerObjects, playersFound, playerColorMap, scene);
    for (let playerId in playerObjects){
        if (!playersFound[playerId]) {
            scene.remove(playerObjects[playerId]);
            delete playerObjects[playerId];
        }
    }

    // Loop through all single objects
    let singlesFound = {};
    processSingleData(data.neutralSingles, singleObjects, singlesFound, playerColorMap, null, scene);
    for (let playerId in playerData) {
        processSingleData(playerData[playerId], singleObjects, singlesFound, playerColorMap, playerId, scene);
    }

    // Check to see if some single should still exist. Remove the singles if that is the case.
    for (let singleId in singleObjects){
        if (!singlesFound[singleId]) {
            scene.remove(playerObjects[singleId]);
            delete singleObjects[singleId];
        }
    }
})

// Animation setup
var lastTime = Date.now();
function animate() {
    requestAnimationFrame( animate );
    
    // Raycaster to find the click point
    raycaster.setFromCamera( pointer, camera );
    const intersect = raycaster.intersectObject(box);
    if (intersect !== null) {
        casterMarker.position.x = intersect[0].point.x;
        casterMarker.position.y = intersect[0].point.y;
        movePosition = {x: intersect[0].point.x, y: intersect[0].point.y}
    }
    let intersects = [];
    intersects = raycaster.intersectObjects(clickableSingles);
    if (intersects.length > 0) {
        // console.log(clickableSinglesToPlayerId[intersects[0].object.uuid]);
    }

    // Raycaster to find the clicking
    let currTime = Date.now();
    let timeDelta = (currTime - lastTime) / 1000;

    // Update the camera position
    if (selfID in playerObjects) {
        // Modify the zoom behavior
        if (cameraRemainingZoomTime > 0) {
            let zoomTime = Math.min(cameraRemainingZoomTime, timeDelta)
            cameraDistance = cameraDistance + (cameraTowardDistance - cameraDistance) * zoomTime / cameraRemainingZoomTime;
            cameraRemainingZoomTime -= zoomTime;
        }

        // Change the camera behavior
        var cameraOffsetX = 0;
        var cameraOffsetY = -Math.cos(CAMERA_BASE_ANGLE) * cameraDistance;
        var cameraOffsetZ = Math.sin(CAMERA_BASE_ANGLE) * cameraDistance;
        camera.position.x = playerObjects[selfID].position.x + cameraOffsetX;
        camera.position.y = playerObjects[selfID].position.y + cameraOffsetY;
        camera.position.z = cameraOffsetZ;
    }
    stats.update();
    renderer.render( scene, camera );
    lastTime = currTime;
};
animate();

// Resize
function onWindowResize() {

    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize( window.innerWidth, window.innerHeight );
}
window.addEventListener('resize', onWindowResize);

// Scrolling to zoom in and out
function onWheelEvent(event) {
    if (event.deltaY > 0) {
        cameraTowardDistance *= CAMERA_ZOOM_COEFFICIENT;
        cameraRemainingZoomTime = CAMERA_ZOOM_TIME;
    } else {
        cameraTowardDistance /= CAMERA_ZOOM_COEFFICIENT;
        cameraRemainingZoomTime = CAMERA_ZOOM_TIME;
    }
}
window.addEventListener("wheel", onWheelEvent);
