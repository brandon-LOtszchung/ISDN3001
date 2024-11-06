class WebSocketHandler {
    constructor(onDataCallback) {
        this.ws = null;
        this.onDataCallback = onDataCallback;
        this.messageCount = 0;
        this.lastSecond = Date.now();
        this.connect();
    }

    connect() {
        const hostname = window.location.hostname || 'localhost';
        this.ws = new WebSocket(`ws://${hostname}:81`);
        
        this.ws.onopen = () => {
            document.getElementById('status').textContent = 'Connected';
            document.getElementById('status').className = 'connected';
            document.getElementById('ip').textContent = hostname;
            console.log('Connected to WebSocket');
        };
        
        this.ws.onclose = () => {
            document.getElementById('status').textContent = 'Disconnected';
            document.getElementById('status').className = 'disconnected';
            console.log('Disconnected from WebSocket');
            setTimeout(() => this.connect(), 2000);
        };
        
        this.ws.onmessage = (event) => {
            this.updateRate();
            try {
                const data = JSON.parse(event.data);
                this.onDataCallback(data);
                
                // Update display values
                document.getElementById('pitch').textContent = data.pitch.toFixed(2);
                document.getElementById('roll').textContent = data.roll.toFixed(2);
                document.getElementById('yaw').textContent = data.yaw.toFixed(2);
                document.getElementById('raw').textContent = JSON.stringify(data, null, 2);
            } catch (e) {
                console.error('Error parsing data:', e);
            }
        };
    }

    updateRate() {
        this.messageCount++;
        const now = Date.now();
        if (now - this.lastSecond >= 1000) {
            document.getElementById('updateRate').textContent = this.messageCount;
            this.messageCount = 0;
            this.lastSecond = now;
        }
    }
}