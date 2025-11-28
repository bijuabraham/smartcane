import { useMemo } from 'react';
import { Bluetooth, BluetoothOff, Activity, Radio, CreditCard, Battery, Gauge, Settings, Cpu, Wifi, AlertTriangle, User, Bell, Clock, Target } from 'lucide-react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer, ReferenceLine } from 'recharts';

export function Dashboard({ mode, connected, sensorData, sensorHistory, chartStartTime, alerts = [], onConnect, onDisconnect, onConfigOpen, onCalibrationOpen, onModeChange }) {
  const formatValue = (value) => {
    if (value === null || value === undefined) return 'N/A';
    if (typeof value === 'number') return value.toFixed(2);
    return value;
  };

  const chartData = useMemo(() => {
    if (!chartStartTime || sensorHistory.length === 0) return [];
    
    return sensorHistory.map(data => ({
      timeSeconds: Math.round((data.ts - chartStartTime) / 1000),
      ax: data.imu?.ax ?? 0,
      ay: data.imu?.ay ?? 0,
      az: data.imu?.az ?? 0,
    }));
  }, [sensorHistory, chartStartTime]);

  const getAlertIcon = (alertType) => {
    switch (alertType) {
      case 'SOS_BUTTON_PRESSED':
      case 'SOS':
        return <Bell className="text-red-400" size={20} />;
      case 'FALL_DETECTED':
      case 'FALL':
        return <User className="text-orange-400" size={20} />;
      case 'OBSTACLE_NEAR':
      case 'OBSTACLE':
        return <AlertTriangle className="text-yellow-400" size={20} />;
      case 'RFID_SEEN':
      case 'RFID':
        return <CreditCard className="text-green-400" size={20} />;
      default:
        return <Activity className="text-gray-400" size={20} />;
    }
  };

  const getAlertTitle = (alert) => {
    const eventType = alert.event || alert.type;
    switch (eventType) {
      case 'SOS_BUTTON_PRESSED':
      case 'SOS':
        return 'SOS Alert';
      case 'FALL_DETECTED':
      case 'FALL':
        return 'Fall Detected';
      case 'OBSTACLE_NEAR':
      case 'OBSTACLE':
        return 'Obstacle Detected';
      case 'RFID_SEEN':
      case 'RFID':
        return 'RFID Scan';
      default:
        return 'Alert';
    }
  };

  const getAlertDetails = (alert) => {
    const eventType = alert.event || alert.type;
    switch (eventType) {
      case 'FALL_DETECTED':
      case 'FALL':
        return alert.ax ? `Acceleration: ${alert.ax.toFixed(2)}g` : 'High impact detected';
      case 'OBSTACLE_NEAR':
      case 'OBSTACLE':
        return alert.dist_mm ? `Distance: ${alert.dist_mm}mm` : 'Object nearby';
      case 'RFID_SEEN':
      case 'RFID':
        return alert.uid ? `UID: ${alert.uid}` : 'Tag scanned';
      case 'SOS_BUTTON_PRESSED':
      case 'SOS':
        return 'Emergency button pressed';
      default:
        return '';
    }
  };

  const getAlertColor = (alertType) => {
    switch (alertType) {
      case 'SOS_BUTTON_PRESSED':
      case 'SOS':
        return 'border-red-500/50 bg-red-500/10';
      case 'FALL_DETECTED':
      case 'FALL':
        return 'border-orange-500/50 bg-orange-500/10';
      case 'OBSTACLE_NEAR':
      case 'OBSTACLE':
        return 'border-yellow-500/50 bg-yellow-500/10';
      case 'RFID_SEEN':
      case 'RFID':
        return 'border-green-500/50 bg-green-500/10';
      default:
        return 'border-gray-500/50 bg-gray-500/10';
    }
  };

  return (
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-blue-900 to-gray-900 text-white">
      <div className="max-w-[1800px] mx-auto px-4 py-6 sm:px-6 lg:px-8">
        <div className="mb-8">
          <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-4">
            <div>
              <h1 className="text-3xl sm:text-4xl font-bold bg-gradient-to-r from-blue-400 to-purple-400 bg-clip-text text-transparent">
                Smart Walking Stick
              </h1>
              <p className="text-gray-400 mt-1">Real-time monitoring dashboard</p>
            </div>
            
            <div className="flex flex-col sm:flex-row gap-3">
              <div className="flex items-center gap-2 bg-gray-800/70 rounded-lg p-1 border border-gray-700">
                <button
                  onClick={() => onModeChange('hardware')}
                  disabled={connected}
                  className={`flex items-center gap-2 px-3 py-2 rounded-md transition-all text-sm ${
                    mode === 'hardware'
                      ? 'bg-blue-600 text-white'
                      : 'text-gray-400 hover:text-white hover:bg-gray-700'
                  } ${connected ? 'opacity-50 cursor-not-allowed' : ''}`}
                >
                  <Wifi size={16} />
                  <span>Hardware</span>
                </button>
                <button
                  onClick={() => onModeChange('simulator')}
                  disabled={connected}
                  className={`flex items-center gap-2 px-3 py-2 rounded-md transition-all text-sm ${
                    mode === 'simulator'
                      ? 'bg-purple-600 text-white'
                      : 'text-gray-400 hover:text-white hover:bg-gray-700'
                  } ${connected ? 'opacity-50 cursor-not-allowed' : ''}`}
                >
                  <Cpu size={16} />
                  <span>Simulator</span>
                </button>
              </div>
              
              <div className="flex gap-3">
              {connected && (
                <>
                  <button
                    onClick={onCalibrationOpen}
                    className="flex items-center gap-2 px-4 py-2 bg-orange-600 hover:bg-orange-500 rounded-lg transition-colors"
                    title="Calibrate fall detection"
                  >
                    <Target size={20} />
                    <span className="hidden sm:inline">Calibrate</span>
                  </button>
                  <button
                    onClick={onConfigOpen}
                    className="flex items-center gap-2 px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded-lg transition-colors"
                  >
                    <Settings size={20} />
                    <span className="hidden sm:inline">Config</span>
                  </button>
                </>
              )}
                <button
                  onClick={connected ? onDisconnect : onConnect}
                  className={`flex items-center gap-2 px-6 py-2 rounded-lg font-medium transition-all ${
                    connected
                      ? 'bg-red-600 hover:bg-red-700'
                      : mode === 'simulator' 
                        ? 'bg-purple-600 hover:bg-purple-700'
                        : 'bg-blue-600 hover:bg-blue-700'
                  }`}
                >
                  {mode === 'simulator' ? <Cpu size={20} /> : <Bluetooth size={20} />}
                  <span>{connected ? 'Disconnect' : 'Connect'}</span>
                </button>
              </div>
            </div>
          </div>
        </div>

        {!connected ? (
          <div className="bg-gray-800/50 backdrop-blur-sm rounded-2xl p-12 text-center border border-gray-700">
            {mode === 'simulator' ? (
              <Cpu size={64} className="mx-auto mb-4 text-purple-400" />
            ) : (
              <Bluetooth size={64} className="mx-auto mb-4 text-blue-400" />
            )}
            <h2 className="text-2xl font-semibold mb-2">Not Connected</h2>
            <p className="text-gray-400 mb-6">
              {mode === 'simulator' 
                ? 'Click Connect to start the simulator and see live demo data'
                : 'Click the Connect button to pair with your Smart Stick via Bluetooth'}
            </p>
            <p className="text-sm text-gray-500">
              {mode === 'simulator'
                ? 'The simulator generates realistic sensor data for testing the dashboard'
                : 'Make sure your device is powered on and Bluetooth is enabled in your browser'}
            </p>
          </div>
        ) : (
          <div className="grid grid-cols-1 lg:grid-cols-[1fr_400px] gap-6">
            <div className="space-y-6">
            <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
              <div className="bg-gray-800/50 backdrop-blur-sm rounded-xl p-5 border border-gray-700">
                <div className="flex items-center gap-3 mb-2">
                  <Activity className="text-green-400" size={24} />
                  <span className="text-gray-400 text-sm">Acceleration</span>
                </div>
                {sensorData?.imu ? (
                  <div className="space-y-1">
                    <div className="flex justify-between">
                      <span className="text-xs text-gray-500">X:</span>
                      <span className="font-mono">{formatValue(sensorData.imu.ax)}g</span>
                    </div>
                    <div className="flex justify-between">
                      <span className="text-xs text-gray-500">Y:</span>
                      <span className="font-mono">{formatValue(sensorData.imu.ay)}g</span>
                    </div>
                    <div className="flex justify-between">
                      <span className="text-xs text-gray-500">Z:</span>
                      <span className="font-mono">{formatValue(sensorData.imu.az)}g</span>
                    </div>
                  </div>
                ) : (
                  <div className="text-gray-500">No data</div>
                )}
              </div>

              <div className="bg-gray-800/50 backdrop-blur-sm rounded-xl p-5 border border-gray-700">
                <div className="flex items-center gap-3 mb-2">
                  <Gauge className="text-blue-400" size={24} />
                  <span className="text-gray-400 text-sm">Gyroscope</span>
                </div>
                {sensorData?.imu ? (
                  <div className="space-y-1">
                    <div className="flex justify-between">
                      <span className="text-xs text-gray-500">X:</span>
                      <span className="font-mono">{formatValue(sensorData.imu.gx)}°/s</span>
                    </div>
                    <div className="flex justify-between">
                      <span className="text-xs text-gray-500">Y:</span>
                      <span className="font-mono">{formatValue(sensorData.imu.gy)}°/s</span>
                    </div>
                    <div className="flex justify-between">
                      <span className="text-xs text-gray-500">Z:</span>
                      <span className="font-mono">{formatValue(sensorData.imu.gz)}°/s</span>
                    </div>
                  </div>
                ) : (
                  <div className="text-gray-500">No data</div>
                )}
              </div>

              <div className="bg-gray-800/50 backdrop-blur-sm rounded-xl p-5 border border-gray-700">
                <div className="flex items-center gap-3 mb-2">
                  <Radio className="text-yellow-400" size={24} />
                  <span className="text-gray-400 text-sm">Distance</span>
                </div>
                <div className="text-3xl font-bold">
                  {sensorData?.dist_mm !== undefined && sensorData.dist_mm >= 0 
                    ? `${sensorData.dist_mm}mm` 
                    : 'N/A'}
                </div>
              </div>

              {sensorData?.battery ? (
                <div className="bg-gray-800/50 backdrop-blur-sm rounded-xl p-5 border border-gray-700">
                  <div className="flex items-center gap-3 mb-2">
                    <Battery className="text-purple-400" size={24} />
                    <span className="text-gray-400 text-sm">Battery</span>
                  </div>
                  <div className="space-y-1">
                    <div className="text-3xl font-bold">{sensorData.battery.pct}%</div>
                    <div className="text-sm text-gray-500">{formatValue(sensorData.battery.v)}V</div>
                  </div>
                </div>
              ) : null}

              {sensorData?.rfid && (
                <div className="bg-gray-800/50 backdrop-blur-sm rounded-xl p-5 border border-gray-700 sm:col-span-2 lg:col-span-1">
                  <div className="flex items-center gap-3 mb-2">
                    <CreditCard className="text-pink-400" size={24} />
                    <span className="text-gray-400 text-sm">RFID</span>
                  </div>
                  <div className="font-mono text-lg">{sensorData.rfid}</div>
                </div>
              )}
            </div>

            {chartStartTime && (
              <div className="bg-gray-800/50 backdrop-blur-sm rounded-xl p-6 border border-gray-700">
                <h3 className="text-xl font-semibold mb-4">IMU Acceleration History (1 minute window)</h3>
                <ResponsiveContainer width="100%" height={300}>
                  <LineChart data={chartData}>
                    <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                    <XAxis 
                      dataKey="timeSeconds" 
                      stroke="#9CA3AF"
                      tick={{ fill: '#9CA3AF' }}
                      domain={[0, 60]}
                      type="number"
                      tickFormatter={(sec) => `${sec}s`}
                      ticks={[0, 10, 20, 30, 40, 50, 60]}
                      allowDataOverflow={true}
                    />
                    <YAxis 
                      stroke="#9CA3AF"
                      tick={{ fill: '#9CA3AF' }}
                      label={{ value: 'Acceleration (g)', angle: -90, position: 'insideLeft', fill: '#9CA3AF' }}
                      domain={[-2, 12]}
                    />
                    <Tooltip 
                      contentStyle={{ backgroundColor: '#1F2937', border: '1px solid #374151', borderRadius: '8px' }}
                      labelStyle={{ color: '#9CA3AF' }}
                      labelFormatter={(sec) => `Time: ${sec}s`}
                    />
                    <Legend wrapperStyle={{ color: '#9CA3AF' }} />
                    <Line type="monotone" dataKey="ax" stroke="#10B981" name="X" dot={false} isAnimationActive={false} connectNulls={false} />
                    <Line type="monotone" dataKey="ay" stroke="#3B82F6" name="Y" dot={false} isAnimationActive={false} connectNulls={false} />
                    <Line type="monotone" dataKey="az" stroke="#8B5CF6" name="Z" dot={false} isAnimationActive={false} connectNulls={false} />
                  </LineChart>
                </ResponsiveContainer>
              </div>
            )}
          </div>

            <div className="bg-gray-800/50 backdrop-blur-sm rounded-xl border border-gray-700 p-6 h-fit lg:sticky lg:top-6">
              <div className="flex items-center gap-2 mb-4">
                <Bell size={24} className="text-blue-400" />
                <h3 className="text-xl font-semibold">Alert History</h3>
              </div>
              
              {alerts.length === 0 ? (
                <div className="text-center py-12 text-gray-500">
                  <Activity size={48} className="mx-auto mb-3 opacity-30" />
                  <p>No alerts yet</p>
                  <p className="text-sm mt-1">Alerts will appear here as they occur</p>
                </div>
              ) : (
                <div className="space-y-3 max-h-[calc(100vh-200px)] overflow-y-auto pr-2 custom-scrollbar">
                  {[...alerts].reverse().map((alert, index) => {
                    const eventType = alert.event || alert.type;
                    return (
                      <div 
                        key={`${alert.timestamp}-${index}`}
                        className={`p-4 rounded-lg border transition-all hover:scale-[1.02] ${getAlertColor(eventType)}`}
                      >
                        <div className="flex items-start gap-3">
                          <div className="mt-0.5">
                            {getAlertIcon(eventType)}
                          </div>
                          <div className="flex-1 min-w-0">
                            <div className="flex items-center justify-between gap-2 mb-1">
                              <h4 className="font-semibold text-sm">
                                {getAlertTitle(alert)}
                              </h4>
                              <div className="flex items-center gap-1 text-xs text-gray-500 shrink-0">
                                <Clock size={12} />
                                {new Date(alert.timestamp).toLocaleTimeString()}
                              </div>
                            </div>
                            <p className="text-sm text-gray-400">
                              {getAlertDetails(alert)}
                            </p>
                          </div>
                        </div>
                      </div>
                    );
                  })}
                </div>
              )}
            </div>
          </div>
        )}
      </div>
    </div>
  );
}
