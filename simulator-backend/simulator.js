export class SmartStickSimulator {
  constructor() {
    this.config = {
      sensor_period_ms: 200,
      obstacle_threshold_mm: 800,
      fall_ax_threshold: 2.2,
      ble_tx_power: 7,
    };
    
    this.state = {
      battery: 85,
      walking: false,
      obstacleNear: false,
      lastRFID: null,
      sosActive: false,
    };
    
    this.time = 0;
    this.walkCycle = 0;
  }
  
  getSensorData() {
    this.time += this.config.sensor_period_ms / 1000;
    
    const isWalking = Math.random() > 0.3;
    if (isWalking !== this.state.walking) {
      this.state.walking = isWalking;
    }
    
    const accel = this.generateIMUData();
    const gyro = this.generateGyroData();
    const distance = this.generateDistanceData();
    
    this.state.battery = Math.max(20, this.state.battery - 0.0001);
    
    return {
      ts: Date.now(),
      imu: {
        ax: Math.round(accel.x * 100) / 100,
        ay: Math.round(accel.y * 100) / 100,
        az: Math.round(accel.z * 100) / 100,
        gx: Math.round(gyro.x * 10) / 10,
        gy: Math.round(gyro.y * 10) / 10,
        gz: Math.round(gyro.z * 10) / 10,
      },
      dist_mm: distance,
      rfid: this.state.lastRFID,
      battery: {
        v: Math.round((3.7 + (this.state.battery / 100) * 0.5) * 100) / 100,
        pct: Math.round(this.state.battery),
      },
    };
  }
  
  generateIMUData() {
    if (this.state.walking) {
      this.walkCycle += 0.2;
      const step = Math.sin(this.walkCycle);
      
      return {
        x: 0.1 + step * 0.3,
        y: 0.05 + Math.abs(step) * 0.2,
        z: 9.8 + step * 0.5,
      };
    } else {
      return {
        x: (Math.random() - 0.5) * 0.1,
        y: (Math.random() - 0.5) * 0.1,
        z: 9.8 + (Math.random() - 0.5) * 0.2,
      };
    }
  }
  
  generateGyroData() {
    if (this.state.walking) {
      const tilt = Math.sin(this.walkCycle * 0.8);
      return {
        x: tilt * 15,
        y: (Math.random() - 0.5) * 5,
        z: (Math.random() - 0.5) * 3,
      };
    } else {
      return {
        x: (Math.random() - 0.5) * 2,
        y: (Math.random() - 0.5) * 2,
        z: (Math.random() - 0.5) * 1,
      };
    }
  }
  
  generateDistanceData() {
    const baseDistance = 1500;
    
    if (Math.random() > 0.5) {
      this.state.obstacleNear = true;
      return Math.floor(300 + Math.random() * 400);
    } else if (this.state.obstacleNear && Math.random() > 0.3) {
      this.state.obstacleNear = false;
    }
    
    if (this.state.obstacleNear) {
      return Math.floor(400 + Math.random() * 300);
    }
    
    return Math.floor(baseDistance + (Math.random() - 0.5) * 500);
  }
  
  checkForAlerts() {
    if (Math.random() > 0.85 && !this.state.sosActive) {
      const alertType = Math.random();
      
      if (alertType < 0.25) {
        console.log('Simulating FALL alert');
        return {
          type: 'FALL',
          ts: Date.now(),
          ax: 15.5,
        };
      } else if (alertType < 0.7 && this.state.obstacleNear) {
        console.log('Simulating OBSTACLE alert');
        return {
          type: 'OBSTACLE',
          ts: Date.now(),
          dist_mm: 350,
        };
      } else if (alertType < 0.95) {
        const rfidId = 'RFID_' + Math.floor(Math.random() * 1000).toString().padStart(4, '0');
        this.state.lastRFID = rfidId;
        console.log('Simulating RFID scan:', rfidId);
        return {
          type: 'RFID',
          ts: Date.now(),
          uid: rfidId,
        };
      }
    }
    
    return null;
  }
  
  triggerSOS() {
    this.state.sosActive = true;
    setTimeout(() => {
      this.state.sosActive = false;
    }, 5000);
    
    return {
      type: 'SOS',
      ts: Date.now(),
    };
  }
  
  getConfig() {
    return { ...this.config };
  }
  
  updateConfig(newConfig) {
    this.config = {
      sensor_period_ms: newConfig.sensor_period_ms ?? this.config.sensor_period_ms,
      obstacle_threshold_mm: newConfig.obstacle_threshold_mm ?? this.config.obstacle_threshold_mm,
      fall_ax_threshold: newConfig.fall_ax_threshold ?? this.config.fall_ax_threshold,
      ble_tx_power: newConfig.ble_tx_power ?? this.config.ble_tx_power,
    };
    
    return { ok: true };
  }
  
  stop() {
    console.log('Simulator stopped');
  }
}
