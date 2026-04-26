# ESP32 Smart Lamp (IoT)

Real-time smart lamp control using ESP32 + Node.js + WebSocket.

## Features
- Touch control (ESP32)
- Web UI (buttons + slider)
- Real-time sync (WebSocket)
- Works locally (WiFi)

## Tech Stack
- ESP32 (Arduino)
- Node.js (Express + WebSocket)
- HTML/CSS/JS frontend

## How it works
ESP32 connects to WiFi → WebSocket → Node.js server → Web UI

## Setup
1. Run server:
   cd server
   npm install
   node server.js

2. Upload ESP32 code (update WiFi + IP)

3. Open:
   http://localhost:3000