const express = require("express");
const http = require("http");
const WebSocket = require("ws");

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

let currentValue = "0";

// WebSocket
wss.on("connection", (ws) => {
  console.log("Client connected");

  // send current state
  ws.send(currentValue);

  ws.on("message", (msg) => {
    currentValue = msg.toString();
    console.log("New Value:", currentValue);

    // broadcast to all
    wss.clients.forEach(client => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(currentValue);
      }
    });
  });
});

// serve UI
app.use(express.static(__dirname));

server.listen(3000, () => {
  console.log("Server running http://localhost:3000");
});