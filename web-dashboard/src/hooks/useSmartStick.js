import { useState, useEffect, useRef } from 'react';
import { SmartStickBLE } from '../services/bluetooth';
import { SmartStickSimulator } from '../services/simulator';
import toast from 'react-hot-toast';

const CHART_WINDOW_MS = 60000; // 60 seconds

export function useSmartStick() {
  const [mode, setMode] = useState('simulator');
  const [connected, setConnected] = useState(false);
  const [sensorData, setSensorData] = useState(null);
  const [alerts, setAlerts] = useState([]);
  const [sensorHistory, setSensorHistory] = useState([]);
  const [currentConfig, setCurrentConfig] = useState(null);
  const [chartStartTime, setChartStartTime] = useState(null);
  const deviceRef = useRef(null);
  const chartStartTimeRef = useRef(null);

  useEffect(() => {
    if (mode === 'hardware') {
      deviceRef.current = new SmartStickBLE();
    } else {
      deviceRef.current = new SmartStickSimulator();
    }
    
    const handleSensorData = (data) => {
      const now = Date.now();
      
      // Check if we've exceeded 60 seconds - reset chart
      if (chartStartTimeRef.current && (now - chartStartTimeRef.current) >= CHART_WINDOW_MS) {
        chartStartTimeRef.current = now;
        setChartStartTime(now);
        setSensorHistory([]);
      }
      
      // Add receive timestamp for consistent charting
      const dataWithTimestamp = {
        ...data,
        ts: now
      };
      
      // Preserve battery data between updates (battery only sent every 10 seconds)
      setSensorData(prev => {
        const merged = { ...dataWithTimestamp };
        if (!merged.battery && prev?.battery) {
          merged.battery = prev.battery;
        }
        return merged;
      });
      
      setSensorHistory(prev => [...prev, dataWithTimestamp]);
    };

    const handleAlert = (alert) => {
      setAlerts(prev => [...prev, { ...alert, timestamp: Date.now() }]);
      
      const eventType = alert.event || alert.type;
      
      switch (eventType) {
        case 'SOS_BUTTON_PRESSED':
        case 'SOS':
          toast.error('🚨 SOS Alert!', { duration: 5000 });
          break;
        case 'FALL_DETECTED':
        case 'FALL':
          toast.error(`🤕 Fall Detected!`, { duration: 5000 });
          break;
        case 'OBSTACLE_NEAR':
        case 'OBSTACLE':
          toast.warning(`⚠️ Obstacle: ${alert.dist_mm}mm`, { duration: 3000 });
          break;
        case 'RFID_SEEN':
        case 'RFID':
          toast.success(`📍 RFID: ${alert.uid}`, { duration: 3000 });
          break;
      }
    };
    
    deviceRef.current.onSensorData(handleSensorData);
    deviceRef.current.onAlert(handleAlert);
    
    if (mode === 'hardware') {
      deviceRef.current.onDisconnect = () => {
        setConnected(false);
        setSensorData(null);
        toast.error('Disconnected from Smart Stick');
      };
    }

    return () => {
      if (deviceRef.current) {
        deviceRef.current.disconnect();
      }
    };
  }, [mode]);

  const connect = async () => {
    try {
      await deviceRef.current.connect();
      setConnected(true);
      const now = Date.now();
      chartStartTimeRef.current = now;
      setChartStartTime(now);
      setSensorHistory([]);
      
      try {
        const config = await deviceRef.current.readConfig();
        setCurrentConfig(config);
      } catch (err) {
        console.warn('Could not read initial config:', err);
      }
      
      const deviceType = mode === 'hardware' ? 'Hardware' : 'Simulator';
      toast.success(`Connected to ${deviceType}`);
    } catch (error) {
      toast.error(error.message || 'Failed to connect');
      throw error;
    }
  };

  const disconnect = async () => {
    await deviceRef.current.disconnect();
    setConnected(false);
    setSensorData(null);
    setSensorHistory([]);
    setCurrentConfig(null);
    chartStartTimeRef.current = null;
    setChartStartTime(null);
  };

  const updateConfig = async (config) => {
    try {
      const response = await deviceRef.current.updateConfig(config);
      if (response.ok) {
        setCurrentConfig(prev => ({ ...prev, ...config }));
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
  
  const switchMode = async (newMode) => {
    if (connected) {
      await disconnect();
    }
    setMode(newMode);
    toast.success(`Switched to ${newMode === 'hardware' ? 'Hardware' : 'Simulator'} mode`);
  };

  return {
    mode,
    connected,
    sensorData,
    alerts,
    sensorHistory,
    currentConfig,
    chartStartTime,
    connection: deviceRef.current,
    connect,
    disconnect,
    updateConfig,
    switchMode,
  };
}
