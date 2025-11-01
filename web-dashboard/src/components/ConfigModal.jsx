import { useState, useEffect } from 'react';
import { X, Save } from 'lucide-react';

export function ConfigModal({ onClose, onSave, initialConfig = {} }) {
  const [config, setConfig] = useState({
    sensor_period_ms: 200,
    obstacle_threshold_mm: 800,
    fall_ax_threshold: 2.2,
    ble_tx_power: 7,
  });

  useEffect(() => {
    if (initialConfig && Object.keys(initialConfig).length > 0) {
      setConfig({
        sensor_period_ms: initialConfig.sensor_period_ms ?? 200,
        obstacle_threshold_mm: initialConfig.obstacle_threshold_mm ?? 800,
        fall_ax_threshold: initialConfig.fall_ax_threshold ?? 2.2,
        ble_tx_power: initialConfig.ble_tx_power ?? 7,
      });
    }
  }, [initialConfig]);

  const handleSubmit = (e) => {
    e.preventDefault();
    onSave(config);
  };

  const updateValue = (key, value) => {
    setConfig(prev => ({ ...prev, [key]: parseFloat(value) || 0 }));
  };

  return (
    <div className="fixed inset-0 bg-black/50 backdrop-blur-sm flex items-center justify-center p-4 z-50">
      <div className="bg-gray-800 rounded-2xl max-w-md w-full p-6 border border-gray-700">
        <div className="flex items-center justify-between mb-6">
          <h2 className="text-2xl font-bold">Device Configuration</h2>
          <button
            onClick={onClose}
            className="p-2 hover:bg-gray-700 rounded-lg transition-colors"
          >
            <X size={24} />
          </button>
        </div>

        <form onSubmit={handleSubmit} className="space-y-4">
          <div>
            <label className="block text-sm font-medium text-gray-300 mb-2">
              Sensor Update Period (ms)
            </label>
            <input
              type="number"
              min="100"
              max="1000"
              step="50"
              value={config.sensor_period_ms}
              onChange={(e) => updateValue('sensor_period_ms', e.target.value)}
              className="w-full bg-gray-700 border border-gray-600 rounded-lg px-4 py-2 text-white focus:outline-none focus:ring-2 focus:ring-blue-500"
            />
            <p className="text-xs text-gray-500 mt-1">Range: 100-1000ms (default: 200ms)</p>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-300 mb-2">
              Obstacle Threshold (mm)
            </label>
            <input
              type="number"
              min="200"
              max="2000"
              step="100"
              value={config.obstacle_threshold_mm}
              onChange={(e) => updateValue('obstacle_threshold_mm', e.target.value)}
              className="w-full bg-gray-700 border border-gray-600 rounded-lg px-4 py-2 text-white focus:outline-none focus:ring-2 focus:ring-blue-500"
            />
            <p className="text-xs text-gray-500 mt-1">Range: 200-2000mm (default: 800mm)</p>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-300 mb-2">
              Fall Detection Threshold (g)
            </label>
            <input
              type="number"
              min="0.5"
              max="5.0"
              step="0.1"
              value={config.fall_ax_threshold}
              onChange={(e) => updateValue('fall_ax_threshold', e.target.value)}
              className="w-full bg-gray-700 border border-gray-600 rounded-lg px-4 py-2 text-white focus:outline-none focus:ring-2 focus:ring-blue-500"
            />
            <p className="text-xs text-gray-500 mt-1">Range: 0.5-5.0g (default: 2.2g)</p>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-300 mb-2">
              BLE TX Power (dBm)
            </label>
            <select
              value={config.ble_tx_power}
              onChange={(e) => updateValue('ble_tx_power', e.target.value)}
              className="w-full bg-gray-700 border border-gray-600 rounded-lg px-4 py-2 text-white focus:outline-none focus:ring-2 focus:ring-blue-500"
            >
              <option value="-12">-12 dBm (lowest)</option>
              <option value="-9">-9 dBm</option>
              <option value="-6">-6 dBm</option>
              <option value="-3">-3 dBm</option>
              <option value="0">0 dBm</option>
              <option value="3">3 dBm</option>
              <option value="6">6 dBm</option>
              <option value="7">7 dBm (default)</option>
              <option value="9">9 dBm (highest)</option>
            </select>
            <p className="text-xs text-gray-500 mt-1">Higher power = longer range, more battery usage</p>
          </div>

          <div className="flex gap-3 pt-4">
            <button
              type="button"
              onClick={onClose}
              className="flex-1 px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded-lg transition-colors"
            >
              Cancel
            </button>
            <button
              type="submit"
              className="flex-1 flex items-center justify-center gap-2 px-4 py-2 bg-blue-600 hover:bg-blue-700 rounded-lg transition-colors font-medium"
            >
              <Save size={20} />
              Save
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
