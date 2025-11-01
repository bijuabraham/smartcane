export class SmartStickSimulator {
  constructor() {
    this.ws = null;
    this.onSensorDataCallback = null;
    this.onAlertCallback = null;
    this.connected = false;
    this.currentConfig = null;
  }

  async connect() {
    return new Promise((resolve, reject) => {
      try {
        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        const wsUrl = `${protocol}//${window.location.host}/simulator`;
        
        console.log('Connecting to simulator:', wsUrl);
        this.ws = new WebSocket(wsUrl);
        
        this.ws.onopen = () => {
          console.log('Connected to simulator backend');
          this.connected = true;
          
          this.ws.send(JSON.stringify({ type: 'getConfig' }));
          resolve();
        };
        
        this.ws.onmessage = (event) => {
          try {
            const message = JSON.parse(event.data);
            
            if (message.type === 'sensorData' && this.onSensorDataCallback) {
              this.onSensorDataCallback(message.data);
            } else if (message.type === 'alert' && this.onAlertCallback) {
              this.onAlertCallback(message.data);
            } else if (message.type === 'config') {
              this.currentConfig = message.data;
            } else if (message.type === 'configResponse') {
              console.log('Config updated:', message.data);
            }
          } catch (error) {
            console.error('Error parsing simulator message:', error);
          }
        };
        
        this.ws.onerror = (error) => {
          console.error('Simulator WebSocket error:', error);
          reject(new Error('Failed to connect to simulator'));
        };
        
        this.ws.onclose = () => {
          console.log('Disconnected from simulator');
          this.connected = false;
        };
        
      } catch (error) {
        reject(error);
      }
    });
  }

  onSensorData(callback) {
    this.onSensorDataCallback = callback;
  }

  onAlert(callback) {
    this.onAlertCallback = callback;
  }

  async disconnect() {
    if (this.ws) {
      this.ws.close();
      this.ws = null;
    }
    this.connected = false;
  }

  async readConfig() {
    if (!this.connected) {
      throw new Error('Not connected to simulator');
    }
    
    return new Promise((resolve) => {
      this.ws.send(JSON.stringify({ type: 'getConfig' }));
      
      const checkConfig = setInterval(() => {
        if (this.currentConfig) {
          clearInterval(checkConfig);
          resolve(this.currentConfig);
        }
      }, 100);
      
      setTimeout(() => {
        clearInterval(checkConfig);
        resolve(this.currentConfig || {
          sensor_period_ms: 200,
          obstacle_threshold_mm: 800,
          fall_ax_threshold: 2.2,
          ble_tx_power: 7,
        });
      }, 2000);
    });
  }

  async updateConfig(config) {
    if (!this.connected) {
      throw new Error('Not connected to simulator');
    }
    
    this.ws.send(JSON.stringify({
      type: 'updateConfig',
      config: config
    }));
    
    this.currentConfig = config;
    
    return { ok: true };
  }

  triggerSOS() {
    if (this.connected && this.ws) {
      this.ws.send(JSON.stringify({ type: 'triggerSOS' }));
    }
  }

  isConnected() {
    return this.connected;
  }
}
