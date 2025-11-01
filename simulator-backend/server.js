import express from 'express';
import { WebSocketServer } from 'ws';
import cors from 'cors';
import { SmartStickSimulator } from './simulator.js';

const app = express();
const PORT = 3001;

app.use(cors());
app.use(express.json());

const server = app.listen(PORT, '0.0.0.0', () => {
  console.log(`Smart Stick Simulator running on port ${PORT}`);
});

const wss = new WebSocketServer({ server });

wss.on('connection', (ws) => {
  console.log('Client connected to simulator');
  
  const simulator = new SmartStickSimulator();
  let messageCount = 0;
  
  ws.on('message', (message) => {
    try {
      const data = JSON.parse(message.toString());
      console.log('Received message:', data.type);
      
      if (data.type === 'getConfig') {
        ws.send(JSON.stringify({
          type: 'config',
          data: simulator.getConfig()
        }));
      } else if (data.type === 'updateConfig') {
        const result = simulator.updateConfig(data.config);
        ws.send(JSON.stringify({
          type: 'configResponse',
          data: result
        }));
      } else if (data.type === 'triggerSOS') {
        simulator.triggerSOS();
      }
    } catch (error) {
      console.error('Error processing message:', error);
    }
  });
  
  const dataInterval = setInterval(() => {
    if (ws.readyState === ws.OPEN) {
      messageCount++;
      if (messageCount % 25 === 0) {
        console.log(`Sent ${messageCount} sensor data messages`);
      }
      ws.send(JSON.stringify({
        type: 'sensorData',
        data: simulator.getSensorData()
      }));
    }
  }, 200);
  
  const alertInterval = setInterval(() => {
    if (ws.readyState === ws.OPEN) {
      const alert = simulator.checkForAlerts();
      if (alert) {
        ws.send(JSON.stringify({
          type: 'alert',
          data: alert
        }));
      }
    }
  }, 1000);
  
  ws.on('close', () => {
    console.log('Client disconnected');
    clearInterval(dataInterval);
    clearInterval(alertInterval);
    simulator.stop();
  });
});

app.get('/health', (req, res) => {
  res.json({ status: 'ok', simulator: 'running' });
});

console.log('WebSocket server ready for connections');
