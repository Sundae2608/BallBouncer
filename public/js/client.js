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
      var s = randomInt(42, 98);
      var l = randomInt(40, 40);
      return `hsl(${h},${s}%,${l}%)`;
    };
  })();

function processPlayerData(playerData, playerObjects, playersFound, playerColorMap, scene) {
    for (let playerId in playerData) {
        playersFound[playerId] = true;
        let pos = new Float64Array(playerData[playerId].position);
        if (!(playerId in playerObjects)) {
            // Pick a color for the player
            const color = randomColor();
            playerColorMap[playerId] = color;

            // Create an object with that color
            const geometry = new THREE.SphereGeometry( 2, 40, 40 );
            const material = new THREE.MeshBasicMaterial( {color: color} );
            const obj = new THREE.Mesh( geometry, material );
            scene.add(obj);
            playerObjects[playerId] = obj;
        }
        let playerObject = playerObjects[playerId];
        playerObject.position.x = pos[0];
        playerObject.position.y = pos[1];
        playerObject.position.z = 1;
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
            const material = new THREE.MeshBasicMaterial( {color: 0x696969} );
            const obj = new THREE.Mesh( geometry, material );
            scene.add(obj);
            singleObjects[singleId] = obj;
        }
        let singleObject = singleObjects[singleId];
        singleObject.position.x = singlePosition[0];
        singleObject.position.y = singlePosition[1];
        singleObject.position.z = 1;
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
const gui = new GUI();
let folderLocal = gui.addFolder( 'Local Clipping' );
let propsLocal = {

    get 'Enabled'() {
        return 10;
    },
    set 'Enabled'( v ) {
        return 10;
    },

    get 'Shadows'() {
        return 10;
    },
    set 'Shadows'( v ) {
        return 10;
    },

    get 'Plane'() {
        return 10;
    },
    set 'Plane'( v ) {
        return 10;
    }
};

let folderGlobal = gui.addFolder( 'Global Clipping' );
let propsGlobal = {
    get 'Enabled'() {
        return 10;
    },
    set 'Enabled'( v ) {
        return 10;
    },
    get 'Plane'() {
        return;
    },
    set 'Plane'( v ) {
        return;
    }
};

folderLocal.add( propsLocal, 'Enabled' );
folderLocal.add( propsLocal, 'Shadows' );
folderLocal.add( propsLocal, 'Plane', 0.3, 1.25 );

folderGlobal.add( propsGlobal, 'Enabled' );
folderGlobal.add( propsGlobal, 'Plane', - 0.4, 3 );


// Camera set up
const CAMERA_OFFSET_X = 0;
const CAMERA_OFFSET_Y = -600;
const CAMERA_OFFSET_Z = 340;
const camera = new THREE.PerspectiveCamera( 10, window.innerWidth / window.innerHeight, 1, 10000 );
camera.position.set( CAMERA_OFFSET_X, CAMERA_OFFSET_Y, CAMERA_OFFSET_Z );
camera.lookAt(new THREE.Vector3(0, 0, 0));

// Controls
if (CAMERA_CONTROL) {
    const orbitalControl = new OrbitControls( camera, renderer.domElement );
    orbitalControl.damping = 0.2;
    
    const transformControl = new TransformControls( camera, renderer.domElement );
    scene.add( transformControl );
}

// Light setup
scene.background = new THREE.Color( 0xf0f0f0 );		
scene.add( new THREE.AmbientLight( 0xf0f0f0 ) );		
const light = new THREE.SpotLight( 0xffffff, 1.5 );
light.position.set( 0, 0, 200 );
light.angle = Math.PI;
light.castShadow = true;
light.shadow.camera.near = 200;
light.shadow.camera.far = 2000;
light.shadow.bias = - 0.000222;
light.shadow.mapSize.width = 1024;
light.shadow.mapSize.height = 1024;
scene.add( light );

// Box setup
const boxGeometry = new THREE.BoxGeometry( 2000, 2000, 0.1 );
const boxMaterial = new THREE.MeshBasicMaterial( { color: 0xf0f0f0, opacity: 1.0 } );
const box = new THREE.Mesh( boxGeometry, boxMaterial );
box.position.x = 0;
box.position.y = 0;
box.position.z = -0.1;
box.receiveShadow = true;
scene.add( box );

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
    if (movePosition !== null) {
        socket.emit('move', movePosition);
    }
}
window.addEventListener('pointerdown', onPointerDown)

// Stats setup
const stats = Stats()
document.body.append(stats.domElement)

// Object set up
let selfID;
let playerObjects = {};
let singleObjects = {};
let playerColorMap = {};

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
function animate() {
    requestAnimationFrame( animate );
    
    // Raycaster
    raycaster.setFromCamera( pointer, camera );
    const intersect = raycaster.intersectObject(box);
    if (intersect !== null) {
        casterMarker.position.x = intersect[0].point.x;
        casterMarker.position.y = intersect[0].point.y;
        movePosition = {x: intersect[0].point.x, y: intersect[0].point.y}
    }

    // Update the camera position
    if (selfID in playerObjects) {
        camera.position.x = playerObjects[selfID].position.x + CAMERA_OFFSET_X;
        camera.position.y = playerObjects[selfID].position.y + CAMERA_OFFSET_Y;
    }
    stats.update();
    renderer.render( scene, camera );
};
animate();

// Resize
function onWindowResize() {

    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize( window.innerWidth, window.innerHeight );
}
window.addEventListener('resize', onWindowResize);

