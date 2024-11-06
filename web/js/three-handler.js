class ThreeHandler {
    constructor() {
        this.scene = null;
        this.camera = null;
        this.renderer = null;
        this.cube = null;
        this.controls = null;
        this.init();
    }

    init() {
        // Scene setup
        this.scene = new THREE.Scene();
        this.scene.background = new THREE.Color(0x2c3e50);

        // Camera setup
        const container = document.getElementById('visualizer');
        const aspect = container.clientWidth / container.clientHeight;
        this.camera = new THREE.PerspectiveCamera(75, aspect, 0.1, 1000);
        this.camera.position.z = 5;
        this.camera.position.y = 2;

        // Renderer setup
        this.renderer = new THREE.WebGLRenderer({ antialias: true });
        this.renderer.setSize(container.clientWidth, container.clientHeight);
        container.appendChild(this.renderer.domElement);

        // Create cube
        const geometry = new THREE.BoxGeometry(2, 0.5, 1);
        const material = new THREE.MeshPhongMaterial({
            color: 0x00ff00,
            wireframe: false,
            transparent: true,
            opacity: 0.9
        });
        this.cube = new THREE.Mesh(geometry, material);
        this.scene.add(this.cube);

        // Add axes helper
        const axesHelper = new THREE.AxesHelper(3);
        this.scene.add(axesHelper);

        // Add grid
        const gridHelper = new THREE.GridHelper(10, 10);
        this.scene.add(gridHelper);

        // Add lights
        const ambientLight = new THREE.AmbientLight(0x404040);
        this.scene.add(ambientLight);

        const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
        directionalLight.position.set(5, 5, 5);
        this.scene.add(directionalLight);

        // OrbitControls setup
        this.controls = new THREE.OrbitControls(this.camera, this.renderer.domElement);
        this.controls.enableDamping = true;
        this.controls.dampingFactor = 0.05;

        // Handle window resize
        window.addEventListener('resize', () => this.onWindowResize(), false);
    }

    onWindowResize() {
        const container = document.getElementById('visualizer');
        const aspect = container.clientWidth / container.clientHeight;
        this.camera.aspect = aspect;
        this.camera.updateProjectionMatrix();
        this.renderer.setSize(container.clientWidth, container.clientHeight);
    }

    animate() {
        requestAnimationFrame(() => this.animate());
        this.controls.update();
        this.renderer.render(this.scene, this.camera);
    }

    updateRotation(pitch, roll, yaw) {
        if (this.cube) {
            const pitchRad = THREE.MathUtils.degToRad(pitch);
            const rollRad = THREE.MathUtils.degToRad(roll);
            const yawRad = THREE.MathUtils.degToRad(yaw);
            this.cube.rotation.set(pitchRad, yawRad, rollRad, 'XYZ');
        }
    }

    resetView() {
        this.camera.position.set(0, 2, 5);
        this.camera.lookAt(0, 0, 0);
        this.controls.reset();
    }
}