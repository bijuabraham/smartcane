const SMART_STICK_SERVICE_UUID = '12345678-1234-1234-1234-1234567890ab';
const SENSOR_DATA_CHAR_UUID = '12345678-1234-1234-1234-1234567890ad';
const ALERTS_CHAR_UUID = '12345678-1234-1234-1234-1234567890ae';
const CONFIG_CHAR_UUID = '12345678-1234-1234-1234-1234567890af';
const CALIBRATION_CHAR_UUID = '12345678-1234-1234-1234-1234567890b0';

export class SmartStickBLE {
  constructor() {
    this.device = null;
    this.server = null;
    this.service = null;
    this.sensorDataChar = null;
    this.alertsChar = null;
    this.configChar = null;
    this.calibrationChar = null;
    this.onSensorData = null;
    this.onAlert = null;
    this.onCalibration = null;
    this.onDisconnect = null;
  }

  async connect() {
    try {
      if (!navigator.bluetooth) {
        throw new Error('Web Bluetooth API is not available in this browser');
      }

      this.device = await navigator.bluetooth.requestDevice({
        filters: [{ name: 'SmartStick' }],
        optionalServices: [SMART_STICK_SERVICE_UUID]
      });

      this.device.addEventListener('gattserverdisconnected', () => {
        if (this.onDisconnect) {
          this.onDisconnect();
        }
      });

      this.server = await this.device.gatt.connect();
      this.service = await this.server.getPrimaryService(SMART_STICK_SERVICE_UUID);

      this.sensorDataChar = await this.service.getCharacteristic(SENSOR_DATA_CHAR_UUID);
      this.alertsChar = await this.service.getCharacteristic(ALERTS_CHAR_UUID);
      this.configChar = await this.service.getCharacteristic(CONFIG_CHAR_UUID);
      this.calibrationChar = await this.service.getCharacteristic(CALIBRATION_CHAR_UUID);

      await this.sensorDataChar.startNotifications();
      await this.configChar.startNotifications();
      await this.calibrationChar.startNotifications();
      
      this.calibrationChar.addEventListener('characteristicvaluechanged', (event) => {
        const value = new TextDecoder().decode(event.target.value);
        try {
          const data = JSON.parse(value);
          if (this.onCalibration) {
            this.onCalibration(data);
          }
        } catch (e) {
          console.error('Failed to parse calibration data:', e);
        }
      });
      this.sensorDataChar.addEventListener('characteristicvaluechanged', (event) => {
        const value = new TextDecoder().decode(event.target.value);
        try {
          const data = JSON.parse(value);
          if (this.onSensorData) {
            this.onSensorData(data);
          }
        } catch (e) {
          console.error('Failed to parse sensor data:', e);
        }
      });

      await this.alertsChar.startNotifications();
      this.alertsChar.addEventListener('characteristicvaluechanged', (event) => {
        const value = new TextDecoder().decode(event.target.value);
        try {
          const data = JSON.parse(value);
          if (this.onAlert) {
            this.onAlert(data);
          }
        } catch (e) {
          console.error('Failed to parse alert:', e);
        }
      });

      return true;
    } catch (error) {
      console.error('Connection failed:', error);
      throw error;
    }
  }

  async disconnect() {
    if (this.device && this.device.gatt.connected) {
      await this.device.gatt.disconnect();
    }
    this.device = null;
    this.server = null;
    this.service = null;
  }

  async readConfig() {
    if (!this.configChar) {
      throw new Error('Not connected to device');
    }

    const value = await this.configChar.readValue();
    const configText = new TextDecoder().decode(value);
    return JSON.parse(configText);
  }

  async updateConfig(config) {
    if (!this.configChar) {
      throw new Error('Not connected to device');
    }

    return new Promise((resolve, reject) => {
      let timeout;
      
      const handleConfigResponse = (event) => {
        clearTimeout(timeout);
        this.configChar.removeEventListener('characteristicvaluechanged', handleConfigResponse);
        
        try {
          const value = new TextDecoder().decode(event.target.value);
          const result = JSON.parse(value);
          
          if (!result || typeof result.ok !== 'boolean') {
            reject(new Error('Invalid response from device'));
          } else {
            resolve(result);
          }
        } catch (e) {
          reject(new Error('Failed to parse config response'));
        }
      };
      
      this.configChar.addEventListener('characteristicvaluechanged', handleConfigResponse);
      
      timeout = setTimeout(() => {
        this.configChar.removeEventListener('characteristicvaluechanged', handleConfigResponse);
        reject(new Error('Config update timeout'));
      }, 5000);
      
      const configJSON = JSON.stringify(config);
      const encoder = new TextEncoder();
      this.configChar.writeValue(encoder.encode(configJSON)).catch(err => {
        clearTimeout(timeout);
        this.configChar.removeEventListener('characteristicvaluechanged', handleConfigResponse);
        reject(err);
      });
    });
  }

  isConnected() {
    return this.device && this.device.gatt && this.device.gatt.connected;
  }
  
  async startCalibration(durationMs = 5000) {
    if (!this.calibrationChar) {
      throw new Error('Not connected to device');
    }
    
    const cmd = JSON.stringify({ cmd: 'start', duration_ms: durationMs });
    const encoder = new TextEncoder();
    await this.calibrationChar.writeValue(encoder.encode(cmd));
  }
  
  async stopCalibration() {
    if (!this.calibrationChar) {
      throw new Error('Not connected to device');
    }
    
    const cmd = JSON.stringify({ cmd: 'stop' });
    const encoder = new TextEncoder();
    await this.calibrationChar.writeValue(encoder.encode(cmd));
  }
  
  async getCalibrationStatus() {
    if (!this.calibrationChar) {
      throw new Error('Not connected to device');
    }
    
    const cmd = JSON.stringify({ cmd: 'status' });
    const encoder = new TextEncoder();
    await this.calibrationChar.writeValue(encoder.encode(cmd));
  }
}
