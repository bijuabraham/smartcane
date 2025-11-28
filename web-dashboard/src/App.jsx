import { useState } from 'react';
import { Toaster, toast } from 'react-hot-toast';
import { Dashboard } from './components/Dashboard';
import { ConfigModal } from './components/ConfigModal';
import CalibrationModal from './components/CalibrationModal';
import { useSmartStick } from './hooks/useSmartStick';

function App() {
  const [showConfig, setShowConfig] = useState(false);
  const [showCalibration, setShowCalibration] = useState(false);
  const { mode, connected, sensorData, sensorHistory, alerts, currentConfig, chartStartTime, connection, connect, disconnect, updateConfig, switchMode } = useSmartStick();

  const handleConfigSave = async (config) => {
    try {
      const response = await updateConfig(config);
      if (response && response.ok) {
        setShowConfig(false);
      }
    } catch (error) {
      console.error('Failed to update config:', error);
    }
  };

  const handleApplyThresholds = async (thresholds) => {
    try {
      console.log('Applying thresholds:', thresholds);
      console.log('Current config:', currentConfig);
      
      const response = await updateConfig(thresholds);
      console.log('Update response:', response);
      
      if (response && response.ok) {
        toast.success('Fall detection thresholds updated!');
      } else {
        toast.error(`Failed to update: ${response?.err || 'Unknown error'}`);
      }
    } catch (error) {
      console.error('Failed to apply thresholds:', error);
      toast.error('Failed to apply thresholds');
    }
  };

  return (
    <>
      <Dashboard
        mode={mode}
        connected={connected}
        sensorData={sensorData}
        sensorHistory={sensorHistory}
        chartStartTime={chartStartTime}
        alerts={alerts}
        onConnect={connect}
        onDisconnect={disconnect}
        onConfigOpen={() => setShowConfig(true)}
        onCalibrationOpen={() => setShowCalibration(true)}
        onModeChange={switchMode}
      />
      
      {showConfig && (
        <ConfigModal
          onClose={() => setShowConfig(false)}
          onSave={handleConfigSave}
          initialConfig={currentConfig}
        />
      )}

      <CalibrationModal
        isOpen={showCalibration}
        onClose={() => setShowCalibration(false)}
        connection={connection}
        onApplyThresholds={handleApplyThresholds}
      />

      <Toaster
        position="bottom-left"
        toastOptions={{
          style: {
            background: '#1F2937',
            color: '#fff',
            border: '1px solid #374151',
          },
        }}
      />
    </>
  );
}

export default App;
