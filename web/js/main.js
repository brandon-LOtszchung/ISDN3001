let threeHandler;
let wsHandler;

function init() {
    threeHandler = new ThreeHandler();
    wsHandler = new WebSocketHandler((data) => {
        threeHandler.updateRotation(data.pitch, data.roll, data.yaw);
    });
    threeHandler.animate();
}

function resetView() {
    threeHandler.resetView();
}

// Start everything when the page loads
document.addEventListener('DOMContentLoaded', init);