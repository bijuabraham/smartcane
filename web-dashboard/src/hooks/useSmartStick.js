import { useState, useEffect, useRef } from 'react';
import { SmartStickBLE } from '../services/bluetooth';
import toast from 'react-hot-toast';

export function useSmartStick() {
  const [connected, setConnected] = useState(false);
  const [sensorData, setSensorData] = useState(null);
  const [alerts, setAlerts] = useState([]);
  const [sensorHistory, setSensorHistory] = useState([]);
  const [currentConfig, setCurrentConfig] = useState(null);
  const bleRef = useRef(null);

  useEffect(() => {
    bleRef.current = new SmartStickBLE();
    
    bleRef.current.onSensorData = (data) => {
      setSensorData(data);
      setSensorHistory(prev => [...prev.slice(-50), data]);
    };

    bleRef.current.onAlert = (alert) => {
      setAlerts(prev => [...prev, { ...alert, timestamp: Date.now() }]);
      
      switch (alert.event) {
        case 'SOS_BUTTON_PRESSED':
          toast.error('🚨 SOS Alert!', { duration: 5000 });
          break;
        case 'FALL_DETECTED':
          toast.error(`🤕 Fall Detected! (${alert.severity})`, { duration: 5000 });
          break;
        case 'OBSTACLE_NEAR':
          toast.warning(`⚠️ Obstacle: ${alert.dist_mm}mm`, { duration: 3000 });
          break;
        case 'RFID_SEEN':
          toast.success(`📍 RFID: ${alert.uid}`, { duration: 3000 });
          break;
      }
    };

    bleRef.current.onDisconnect = () => {
      setConnected(false);
      setSensorData(null);
      toast.error('Disconnected from Smart Stick');
    };

    return () => {
      if (bleRef.current) {
        bleRef.current.disconnect();
      }
    };
  }, []);

  const connect = async () => {
    try {
      await bleRef.current.connect();
      setConnected(true);
      
      try {
        const config = await bleRef.current.readConfig();
        setCurrentConfig(config);
      } catch (err) {
        console.warn('Could not read initial config:', err);
      }
      
      toast.success('Connected to Smart Stick');
    } catch (error) {
      toast.error(error.message || 'Failed to connect');
      throw error;
    }
  };

  const disconnect = async () => {
    await bleRef.current.disconnect();
    setConnected(false);
    setSensorData(null);
    setSensorHistory([]);
    setCurrentConfig(null);
  };

  const updateConfig = async (config) => {
    try {
      const response = await bleRef.current.updateConfig(config);
      if (response.ok) {
        setCurrentConfig(config);
        toast.success('Configuration updated');
      } else {
        toast.error(`Config error: ${response.err}`);
      }
      return response;
    } catch (error) {
      toast.error('Failed to update configuration');
      throw error;
    }
  };

  return {
    connected,
    sensorData,
    alerts,
    sensorHistory,
    currentConfig,
    connect,
    disconnect,
    updateConfig,
  };
}
