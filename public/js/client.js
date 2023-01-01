import * as THREE from "../../node_modules/three/build/three.module.js"
import { OrbitControls } from '../../node_modules/three/examples/jsm/controls/OrbitControls.js'
import { TransformControls } from '../../node_modules/three/examples/jsm/controls/TransformControls.js'

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
const controls = new OrbitControls( camera, renderer.domElement );
controls.damping = 0.2;
controls.addEventListener( 'change', render );

transformControl = new TransformControls( camera, renderer.domElement );
transformControl.addEventListener( 'change', render );
transformControl.addEventListener( 'dragging-changed', function ( event ) {

    controls.enabled = ! event.value;

} );
scene.add( transformControl );

// Light setup
scene.add( new THREE.AmbientLight( 0xf0f0f0 ) );		
scene.background = new THREE.Color( 0xf0f0f0 );		
const light = new THREE.SpotLight( 0xffffff, 1.5 );
light.position.set( 0, 0, 0 );
light.angle = Math.PI;
light.castShadow = true;
light.shadow.camera.near = 200;
light.shadow.camera.far = 2000;
light.shadow.bias = - 0.000222;
light.shadow.mapSize.width = 1024;
light.shadow.mapSize.height = 1024;
scene.add( light );

// Plane setup
const planeGeometry = new THREE.PlaneGeometry( 2000, 2000 );
const planeMaterial = new THREE.ShadowMaterial( { color: 0x000000, opacity: 1.0 } );
const plane = new THREE.Mesh( planeGeometry, planeMaterial );
plane.rotateX( - Math.PI / 2 );
plane.position.z = 0;
plane.receiveShadow = true;
scene.add( plane );

// Helper setup
const helper = new THREE.GridHelper( 2000, 2000 );
helper.position.z = 0;
helper.material.opacity = 0.25;
helper.material.transparent = true;
helper.rotateX( - Math.PI / 2 );
scene.add( helper );

let localPositions = {};
let selfID;
let shapes = {}

socket.on('connect', () => {
    selfID = socket.id;
    localPositions[selfID] = true;
    console.log("We have been connected.");
    socket.emit('newPlayer')
})

socket.on('updatePlayers', playerPositions => {
    let playersFound = {};
    for (let id in playerPositions){
        playersFound[id] = true;
        localPositions[id] = {x: playerPositions[id].x, y: playerPositions[id].y}
        if (!shapes[id]) {
            const geometry = new THREE.SphereGeometry( 1, 10, 10 );
            const material = new THREE.MeshBasicMaterial( {color: 0xffff00} );
            const cube = new THREE.Mesh( geometry, material );
            cube.castShadow = true;
            cube.position.x = playerPositions[id].x;
            cube.position.y = playerPositions[id].y;
            scene.add( cube );
            shapes[id] = cube;
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

function onWindowResize() {

    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();

    renderer.setSize( window.innerWidth, window.innerHeight );

    render();
}

function animate() {
    requestAnimationFrame( animate );

    renderer.render( scene, camera );
};
animate();
