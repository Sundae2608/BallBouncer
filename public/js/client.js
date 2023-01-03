import * as THREE from 'three'
import { OrbitControls } from 'https://unpkg.com/three@0.138.3/examples/jsm/controls/OrbitControls.js'
import { TransformControls } from 'https://unpkg.com/three@0.138.3/examples/jsm/controls/TransformControls.js'
import Stats from 'https://unpkg.com/three@0.138.3/examples/jsm/libs/stats.module'

const socket = io.connect('http://localhost:3000');
const scene = new THREE.Scene();

// Renderer setup
const renderer = new THREE.WebGLRenderer( { antialias: true } );
renderer.shadowMap.enabled = true;
renderer.setSize( window.innerWidth, window.innerHeight );
document.body.appendChild( renderer.domElement );

// Camera set up
const camera = new THREE.PerspectiveCamera( 70, window.innerWidth / window.innerHeight, 1, 10000 );
camera.position.set( 0, 0, 30 );

// Controls
const orbitalControl = new OrbitControls( camera, renderer.domElement );
orbitalControl.damping = 0.2;

const transformControl = new TransformControls( camera, renderer.domElement );
scene.add( transformControl );

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
const boxGeometry = new THREE.BoxGeometry( 2000, 2000, 2 );
const boxMaterial = new THREE.MeshBasicMaterial( { color: 0xf0f0f0, opacity: 1.0 } );
const box = new THREE.Mesh( boxGeometry, boxMaterial );
box.position.x = 0;
box.position.y = 0;
box.position.z = -2;;
box.receiveShadow = true;
scene.add( box );

// Helper setup
const helper = new THREE.GridHelper( 2000, 200 );
helper.position.z = 0;
helper.material.opacity = 0.25;
helper.material.transparent = true;
helper.rotateX( - Math.PI / 2 );
scene.add( helper );

// Raycaster setup
const raycaster = new THREE.Raycaster();
const pointer = new THREE.Vector2();

const casterMarkerGeometry = new THREE.SphereGeometry( 1, 30, 30 );
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
let localPositions = {};
let selfID;
let shapes = {}

socket.on('connect', () => {
    selfID = socket.id;
    localPositions[selfID] = true;
    console.log("We have been connected.");
    socket.emit('newPlayer')
})

const NUM_OBJECTS = 6400;

socket.on('updatePlayers', playerPositions => {
    let playersFound = {};
    for (let id in playerPositions){
        playersFound[id] = true;
        localPositions[id] = {x: playerPositions[id].x, y: playerPositions[id].y}
        if (!shapes[id]) {
            // Create an array of shapes
            shapes[id] = new Array();
            
            // Add a bunch of shape into the cube in random position into the shapes
            for (let i = 0; i < NUM_OBJECTS; i++) {
                const geometry = new THREE.BoxGeometry( 1, 1, 1 );
                const material = new THREE.MeshBasicMaterial( {color: 0xF56028} );
                const cube = new THREE.Mesh( geometry, material );
                cube.castShadow = true;
                cube.position.x = playerPositions[id].x + (Math.random() - 0.5) * 1000;
                cube.position.y = playerPositions[id].y + (Math.random() - 0.5) * 1000;
                scene.add( cube );
                shapes[id].push(cube);
            }
            console.log(shapes.length);
        }
    }
    for (let id in localPositions){
        if (!playersFound[id]) {
            delete localPositions[id];
            scene.remove(shapes[id]);
            delete shapes[id];
        }
    }
})

// Animation setup
function animate() {
    requestAnimationFrame( animate );
    stats.update();
    
    console.log(shapes);
    for (let i = 0; i < NUM_OBJECTS; i++) {
        shapes[selfID][i].position.x += (Math.random() - 0.5);
        shapes[selfID][i].position.y += (Math.random() - 0.5);
    }

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

