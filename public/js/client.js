import * as THREE from 'three'
import { OrbitControls } from 'https://unpkg.com/three@0.138.3/examples/jsm/controls/OrbitControls.js'
import { TransformControls } from 'https://unpkg.com/three@0.138.3/examples/jsm/controls/TransformControls.js'
import Stats from 'https://unpkg.com/three@0.138.3/examples/jsm/libs/stats.module'

const socket = io.connect('http://localhost:3000');
const scene = new THREE.Scene();

// Set up variable
const CAMERA_CONTROL = false;

// Renderer setup
const renderer = new THREE.WebGLRenderer( { antialias: true } );
renderer.shadowMap.enabled = true;
renderer.setSize( window.innerWidth, window.innerHeight );
document.body.appendChild( renderer.domElement );

// Camera set up
const CAMERA_OFFSET_X = 0;
const CAMERA_OFFSET_Y = -300;
const CAMERA_OFFSET_Z = 170;
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
const pointer = new THREE.Vector2();

const casterMarkerGeometry = new THREE.SphereGeometry( 0.4, 40, 40 );
const casterMarkerMaterial = new THREE.MeshBasicMaterial( { color: 0x000000, opacity: 1.0 } );
const casterMarker = new THREE.Mesh( casterMarkerGeometry, casterMarkerMaterial );
scene.add(casterMarker);

casterMarker.position.x = 0;
casterMarker.position.y = 0;
casterMarker.position.z = 0;

var movePosition = null;
function onPointerMove(event) {
    raycaster.setFromCamera( pointer, camera );
	pointer.x = ( event.clientX / window.innerWidth ) * 2 - 1;
	pointer.y = - ( event.clientY / window.innerHeight ) * 2 + 1;
    const intersect = raycaster.intersectObject(box);
    if (intersect !== null) {
        casterMarker.position.x = intersect[0].point.x;
        casterMarker.position.y = intersect[0].point.y;
        movePosition = {x: intersect[0].point.x, y: intersect[0].point.y}
    }
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

// Client setup
let selfID;

let playerObjects = {};
let singleObjects = {};

socket.on('connect', () => {
    selfID = socket.id;
    console.log("We have been connected.");
    socket.emit('newPlayer');
});

socket.on('updatePlayers', data => {
    // Loop through all player objects
    let playersFound = {};
    let playerData = data.playerData;
    for (let playerId in playerData) {
        playersFound[playerId] = true;
        let pos = playerData[playerId].playerPosition;
        if (!(playerId in playerObjects)) {
            const geometry = new THREE.SphereGeometry( 2, 40, 40 );
            const material = new THREE.MeshBasicMaterial( {color: 0xF56028} );
            const obj = new THREE.Mesh( geometry, material );
            scene.add(obj);
            playerObjects[playerId] = obj;
        }
        let playerObject = playerObjects[playerId];
        playerObject.position.x = pos.x;
        playerObject.position.y = pos.y;
        playerObject.position.z = 1;
    }
    for (let playerId in playerObjects){
        if (!playersFound[playerId]) {
            scene.remove(playerObjects[playerId]);
            delete playerObjects[playerId];
        }
    }

    // Loop through all single objects
    let singlesFound = {};
    let singlesData = data.singlesData;
    let singlesPosition = singlesData.singlesPosition;
    let singlesId = new Int32Array(singlesData.singlesId);
    for (let i = 0; i < singlesId.length; i++) {
        let singleId = singlesId[i];
        let singlePosition = new Float64Array(singlesPosition[i]);
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
    }
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

    // Update the camera positio
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

