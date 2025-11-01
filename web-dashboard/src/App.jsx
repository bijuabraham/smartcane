import { useState } from 'react';
import { Toaster } from 'react-hot-toast';
import { Dashboard } from './components/Dashboard';
import { ConfigModal } from './components/ConfigModal';
import { useSmartStick } from './hooks/useSmartStick';

function App() {
  const [showConfig, setShowConfig] = useState(false);
  const { connected, sensorData, sensorHistory, currentConfig, connect, disconnect, updateConfig } = useSmartStick();

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
        connected={connected}
        sensorData={sensorData}
        sensorHistory={sensorHistory}
        onConnect={connect}
        onDisconnect={disconnect}
        onConfigOpen={() => setShowConfig(true)}
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
