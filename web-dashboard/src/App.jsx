import { useState } from 'react';
import { Toaster } from 'react-hot-toast';
import { Dashboard } from './components/Dashboard';
import { ConfigModal } from './components/ConfigModal';
import { useSmartStick } from './hooks/useSmartStick';

function App() {
  const [showConfig, setShowConfig] = useState(false);
  const { mode, connected, sensorData, sensorHistory, alerts, currentConfig, connect, disconnect, updateConfig, switchMode } = useSmartStick();

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

  return (
    <>
      <Dashboard
        mode={mode}
        connected={connected}
        sensorData={sensorData}
        sensorHistory={sensorHistory}
        alerts={alerts}
        onConnect={connect}
        onDisconnect={disconnect}
        onConfigOpen={() => setShowConfig(true)}
        onModeChange={switchMode}
      />
      
      {showConfig && (
        <ConfigModal
          onClose={() => setShowConfig(false)}
          onSave={handleConfigSave}
          initialConfig={currentConfig}
        />
      )}

      <Toaster
        position="top-right"
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
