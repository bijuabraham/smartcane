import { useState, useEffect } from 'react';

export default function CalibrationModal({ isOpen, onClose, connection, onApplyThresholds }) {
  const [calibrationState, setCalibrationState] = useState('idle');
  const [timeRemaining, setTimeRemaining] = useState(0);
  const [results, setResults] = useState(null);
  const [duration, setDuration] = useState(5);

  useEffect(() => {
    if (!connection) return;

    const handleCalibration = (data) => {
      console.log('Calibration data received:', data);
      
      if (data.status === 'started') {
        setCalibrationState('recording');
      } else if (data.status === 'active') {
        setCalibrationState('recording');
      } else if (data.status === 'complete') {
        setCalibrationState('complete');
        setResults(data);
      } else if (data.status === 'idle') {
        if (data.peak_acceleration > 0) {
          setResults(data);
          setCalibrationState('complete');
        }
      }
    };

    if (typeof connection.onCalibration === 'function') {
      connection.onCalibration(handleCalibration);
    }
  }, [connection]);

  useEffect(() => {
    let interval;
    if (calibrationState === 'recording' && timeRemaining > 0) {
      interval = setInterval(() => {
        setTimeRemaining(t => {
          if (t <= 1) {
            return 0;
          }
          return t - 1;
        });
      }, 1000);
    }
    return () => clearInterval(interval);
  }, [calibrationState, timeRemaining]);

  const startCalibration = async () => {
    try {
      setCalibrationState('recording');
      setTimeRemaining(duration);
      setResults(null);
      
      if (connection && connection.startCalibration) {
        await connection.startCalibration(duration * 1000);
      }
    } catch (error) {
      console.error('Failed to start calibration:', error);
      setCalibrationState('idle');
    }
  };

  const stopCalibration = async () => {
    try {
      if (connection && connection.stopCalibration) {
        await connection.stopCalibration();
      }
    } catch (error) {
      console.error('Failed to stop calibration:', error);
    }
  };

  const applyThresholds = () => {
    if (results && onApplyThresholds) {
      onApplyThresholds({
        fall_ax_threshold: Math.round(results.suggested_impact_threshold * 10) / 10,
        fall_motion_threshold: Math.round(results.suggested_motion_threshold * 100) / 100,
      });
    }
    onClose();
  };

  const resetCalibration = () => {
    setCalibrationState('idle');
    setResults(null);
    setTimeRemaining(0);
  };

  if (!isOpen) return null;

  return (
    <div className="fixed inset-0 bg-black/50 backdrop-blur-sm flex items-center justify-center p-4 z-50">
      <div className="bg-gray-800 rounded-2xl max-w-md w-full p-6 border border-gray-700">
        <h2 className="text-xl font-bold text-white mb-4">Fall Calibration</h2>
        
        {calibrationState === 'idle' && (
          <div className="space-y-4">
            <p className="text-gray-300">
              This will record acceleration data while you simulate a fall. The device will track:
            </p>
            <ul className="text-gray-400 text-sm list-disc list-inside space-y-1">
              <li>Peak acceleration (impact spike)</li>
              <li>Minimum motion after impact (stillness)</li>
            </ul>
            
            <div>
              <label className="block text-sm font-medium text-gray-300 mb-2">
                Recording Duration: {duration} seconds
              </label>
              <input
                type="range"
                min="3"
                max="10"
                value={duration}
                onChange={(e) => setDuration(parseInt(e.target.value))}
                className="w-full h-2 bg-gray-700 rounded-lg appearance-none cursor-pointer"
              />
            </div>
            
            <div className="bg-yellow-900/30 border border-yellow-700 rounded-lg p-3">
              <p className="text-yellow-300 text-sm">
                After starting, you have {duration} seconds to perform a fall motion with the device.
              </p>
            </div>
            
            <div className="flex gap-3 mt-6">
              <button
                onClick={onClose}
                className="flex-1 px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded-lg text-white transition-colors"
              >
                Cancel
              </button>
              <button
                onClick={startCalibration}
                className="flex-1 px-4 py-2 bg-blue-600 hover:bg-blue-500 rounded-lg text-white font-medium transition-colors"
              >
                Start Recording
              </button>
            </div>
          </div>
        )}
        
        {calibrationState === 'recording' && (
          <div className="space-y-4 text-center">
            <div className="w-24 h-24 mx-auto relative">
              <svg className="w-full h-full transform -rotate-90">
                <circle
                  cx="48"
                  cy="48"
                  r="44"
                  stroke="#374151"
                  strokeWidth="8"
                  fill="none"
                />
                <circle
                  cx="48"
                  cy="48"
                  r="44"
                  stroke="#3b82f6"
                  strokeWidth="8"
                  fill="none"
                  strokeLinecap="round"
                  strokeDasharray={2 * Math.PI * 44}
                  strokeDashoffset={2 * Math.PI * 44 * (1 - timeRemaining / duration)}
                  className="transition-all duration-1000"
                />
              </svg>
              <div className="absolute inset-0 flex items-center justify-center">
                <span className="text-3xl font-bold text-white">{timeRemaining}</span>
              </div>
            </div>
            
            <div className="animate-pulse">
              <p className="text-red-400 font-medium text-lg">Recording...</p>
              <p className="text-gray-400 text-sm">Perform a fall motion now!</p>
            </div>
            
            <button
              onClick={stopCalibration}
              className="px-6 py-2 bg-red-600 hover:bg-red-500 rounded-lg text-white font-medium transition-colors"
            >
              Stop Early
            </button>
          </div>
        )}
        
        {calibrationState === 'complete' && results && (
          <div className="space-y-4">
            <div className="bg-green-900/30 border border-green-700 rounded-lg p-3">
              <p className="text-green-300 text-sm font-medium">Calibration Complete!</p>
            </div>
            
            <div className="bg-gray-900 rounded-lg p-4 space-y-3">
              <h3 className="text-white font-medium">Recorded Values</h3>
              
              <div className="grid grid-cols-2 gap-3 text-sm">
                <div className="bg-gray-800 rounded p-2">
                  <p className="text-gray-400">Peak Acceleration</p>
                  <p className="text-white text-lg font-mono">{results.peak_acceleration?.toFixed(2)} g</p>
                </div>
                <div className="bg-gray-800 rounded p-2">
                  <p className="text-gray-400">Min Motion</p>
                  <p className="text-white text-lg font-mono">{results.min_motion?.toFixed(2)} g</p>
                </div>
              </div>
              
              <div className="text-xs text-gray-500">
                Peak at X:{results.peak_ax?.toFixed(2)}, Y:{results.peak_ay?.toFixed(2)}, Z:{results.peak_az?.toFixed(2)}
              </div>
            </div>
            
            {results.suggested_impact_threshold && (
              <div className="bg-blue-900/30 border border-blue-700 rounded-lg p-4 space-y-2">
                <h3 className="text-blue-300 font-medium">Suggested Thresholds</h3>
                <div className="grid grid-cols-2 gap-3 text-sm">
                  <div>
                    <p className="text-gray-400">Impact Threshold</p>
                    <p className="text-white font-mono">{results.suggested_impact_threshold?.toFixed(2)} g</p>
                  </div>
                  <div>
                    <p className="text-gray-400">Motion Threshold</p>
                    <p className="text-white font-mono">{results.suggested_motion_threshold?.toFixed(2)} g</p>
                  </div>
                </div>
              </div>
            )}
            
            <div className="flex gap-3 mt-6">
              <button
                onClick={resetCalibration}
                className="flex-1 px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded-lg text-white transition-colors"
              >
                Try Again
              </button>
              {results.suggested_impact_threshold && (
                <button
                  onClick={applyThresholds}
                  className="flex-1 px-4 py-2 bg-green-600 hover:bg-green-500 rounded-lg text-white font-medium transition-colors"
                >
                  Apply Values
                </button>
              )}
              <button
                onClick={onClose}
                className="flex-1 px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded-lg text-white transition-colors"
              >
                Close
              </button>
            </div>
          </div>
        )}
        
        {calibrationState === 'complete' && !results && (
          <div className="space-y-4 text-center">
            <p className="text-gray-400">No fall detected during recording.</p>
            <div className="flex gap-3">
              <button
                onClick={resetCalibration}
                className="flex-1 px-4 py-2 bg-blue-600 hover:bg-blue-500 rounded-lg text-white transition-colors"
              >
                Try Again
              </button>
              <button
                onClick={onClose}
                className="flex-1 px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded-lg text-white transition-colors"
              >
                Close
              </button>
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
