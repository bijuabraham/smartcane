const SMART_STICK_SERVICE_UUID = '12345678-1234-1234-1234-1234567890ab';
const SENSOR_DATA_CHAR_UUID = '12345678-1234-1234-1234-1234567890ad';
const ALERTS_CHAR_UUID = '12345678-1234-1234-1234-1234567890ae';
const CONFIG_CHAR_UUID = '12345678-1234-1234-1234-1234567890af';

export class SmartStickBLE {
  constructor() {
    this.device = null;
    this.server = null;
    this.service = null;
    this.sensorDataChar = null;
    this.alertsChar = null;
    this.configChar = null;
    this.onSensorData = null;
    this.onAlert = null;
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

      await this.sensorDataChar.startNotifications();
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

    const configJSON = JSON.stringify(config);
    const encoder = new TextEncoder();
    await this.configChar.writeValue(encoder.encode(configJSON));

    const response = await this.configChar.readValue();
    const responseText = new TextDecoder().decode(response);
    const result = JSON.parse(responseText);
    
    if (!result || typeof result.ok !== 'boolean') {
      throw new Error('Invalid response from device');
    }
    
    return result;
  }

  isConnected() {
    return this.device && this.device.gatt && this.device.gatt.connected;
  }
}
