import { Bluetooth, BluetoothOff, Activity, Radio, CreditCard, Battery, Gauge, Settings } from 'lucide-react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';

export function Dashboard({ connected, sensorData, sensorHistory, onConnect, onDisconnect, onConfigOpen }) {
  const formatValue = (value) => {
    if (value === null || value === undefined) return 'N/A';
    if (typeof value === 'number') return value.toFixed(2);
    return value;
  };

  return (
    <div className="min-h-screen bg-gradient-to-br from-gray-900 via-blue-900 to-gray-900 text-white">
      <div className="max-w-7xl mx-auto px-4 py-6 sm:px-6 lg:px-8">
        <div className="mb-8">
          <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-4">
            <div>
              <h1 className="text-3xl sm:text-4xl font-bold bg-gradient-to-r from-blue-400 to-purple-400 bg-clip-text text-transparent">
                Smart Walking Stick
              </h1>
              <p className="text-gray-400 mt-1">Real-time monitoring dashboard</p>
            </div>
            
            <div className="flex gap-3">
              {connected && (
                <button
                  onClick={onConfigOpen}
                  className="flex items-center gap-2 px-4 py-2 bg-gray-700 hover:bg-gray-600 rounded-lg transition-colors"
                >
                  <Settings size={20} />
                  <span className="hidden sm:inline">Config</span>
                </button>
              )}
              <button
                onClick={connected ? onDisconnect : onConnect}
                className={`flex items-center gap-2 px-6 py-2 rounded-lg font-medium transition-all ${
                  connected
                    ? 'bg-red-600 hover:bg-red-700'
                    : 'bg-blue-600 hover:bg-blue-700'
                }`}
              >
                {connected ? <BluetoothOff size={20} /> : <Bluetooth size={20} />}
                <span>{connected ? 'Disconnect' : 'Connect'}</span>
              </button>
            </div>
          </div>
        </div>

        {!connected ? (
          <div className="bg-gray-800/50 backdrop-blur-sm rounded-2xl p-12 text-center border border-gray-700">
            <Bluetooth size={64} className="mx-auto mb-4 text-blue-400" />
            <h2 className="text-2xl font-semibold mb-2">Not Connected</h2>
            <p className="text-gray-400 mb-6">Click the Connect button to pair with your Smart Stick</p>
            <p className="text-sm text-gray-500">
              Make sure your device is powered on and Bluetooth is enabled in your browser
            </p>
          </div>
        ) : (
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

            {sensorHistory.length > 0 && (
              <div className="bg-gray-800/50 backdrop-blur-sm rounded-xl p-6 border border-gray-700">
                <h3 className="text-xl font-semibold mb-4">IMU Acceleration History</h3>
                <ResponsiveContainer width="100%" height={300}>
                  <LineChart data={sensorHistory}>
                    <CartesianGrid strokeDasharray="3 3" stroke="#374151" />
                    <XAxis 
                      dataKey="ts" 
                      stroke="#9CA3AF"
                      tick={{ fill: '#9CA3AF' }}
                      tickFormatter={(ts) => new Date(ts).toLocaleTimeString()}
                    />
                    <YAxis 
                      stroke="#9CA3AF"
                      tick={{ fill: '#9CA3AF' }}
                      label={{ value: 'Acceleration (g)', angle: -90, position: 'insideLeft', fill: '#9CA3AF' }}
                    />
                    <Tooltip 
                      contentStyle={{ backgroundColor: '#1F2937', border: '1px solid #374151', borderRadius: '8px' }}
                      labelStyle={{ color: '#9CA3AF' }}
                    />
                    <Legend wrapperStyle={{ color: '#9CA3AF' }} />
                    <Line type="monotone" dataKey="imu.ax" stroke="#10B981" name="X" dot={false} />
                    <Line type="monotone" dataKey="imu.ay" stroke="#3B82F6" name="Y" dot={false} />
                    <Line type="monotone" dataKey="imu.az" stroke="#8B5CF6" name="Z" dot={false} />
                  </LineChart>
                </ResponsiveContainer>
              </div>
            )}
          </div>
        )}
      </div>
    </div>
  );
}
