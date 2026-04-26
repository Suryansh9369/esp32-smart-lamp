#include <WiFi.h>
#include <WebSocketsClient.h>

#define TOUCH_PIN T0
#define LED_PIN 5

const char* ssid = "Xiaomi 11i";
const char* password = "12345679";

// your laptop IP
const char* host = "192.168.112.45";
const int port = 3000;

WebSocketsClient webSocket;

int threshold = 30;  // adjust if needed

int levels[] = {0, 64, 128, 192, 255};
int currentLevel = 0;

bool touched = false;

// ✅ brightness control
void setBrightness(int level) {
  currentLevel = level;
  ledcWrite(LED_PIN, levels[currentLevel]);
}

// ✅ WebSocket events
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  if (type == WStype_CONNECTED) {
    Serial.println("WebSocket Connected");
  }

  if (type == WStype_TEXT) {
    String msg = String((char*)payload);
    Serial.println("Received: " + msg);

    int value = msg.toInt();   // 0–100

    int level = value / 25;    // convert to 0–4
    setBrightness(level);
  }
}

void setup() {
  Serial.begin(115200);

  // ✅ NEW PWM API (IMPORTANT)
  ledcAttach(LED_PIN, 5000, 8);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  webSocket.begin(host, port, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  int value = touchRead(TOUCH_PIN);

  if (value < threshold && !touched) {
    touched = true;

    currentLevel++;
    if (currentLevel >= 5) currentLevel = 0;

    ledcWrite(LED_PIN, levels[currentLevel]);

    // send to server
    char msg[5];
    sprintf(msg, "%d", currentLevel * 25);
    webSocket.sendTXT(msg);

    delay(200);
  }

  if (value >= threshold) {
    touched = false;
  }
}