#include <WiFi.h>
#include <WebSocketsClient.h>
#include <WiFiManager.h>

#define TOUCH_PIN T0      // GPIO 4
#define LED_PIN 5

const char* host = "kismat-ki-batti.onrender.com";

WebSocketsClient webSocket;

// 🔧 adjust based on your readings
int threshold = 700;

// brightness levels
int levels[] = {0, 64, 128, 192, 255};
int currentLevel = 0;

// touch states
unsigned long touchStartTime = 0;
bool longPressTriggered = false;

// ==========================
// SET BRIGHTNESS
// ==========================
void setBrightness(int level) {
  currentLevel = level;
  ledcWrite(LED_PIN, levels[currentLevel]);

  Serial.print("Brightness Level: ");
  Serial.println(currentLevel);
}

// ==========================
// WEBSOCKET EVENTS
// ==========================
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  if (type == WStype_CONNECTED) {
    Serial.println("WebSocket Connected");
  }

  if (type == WStype_TEXT) {
    String msg = String((char*)payload);
    Serial.println("Received: " + msg);

    int value = msg.toInt();
    int level = value / 25;

    setBrightness(level);
  }
}

// ==========================
// SETUP
// ==========================
void setup() {
  Serial.begin(115200);

  // PWM setup
  ledcAttach(LED_PIN, 5000, 8);

  // WiFi Manager
  WiFiManager wm;
  bool res = wm.autoConnect("SmartLamp_Setup", "12345678");

  if (!res) {
    Serial.println("WiFi Failed");
  } else {
    Serial.println("WiFi Connected!");
    Serial.println(WiFi.localIP());
  }

  // WebSocket
  webSocket.beginSSL(host, 443, "/");
  webSocket.onEvent(webSocketEvent);
}

// ==========================
// LOOP
// ==========================
void loop() {

  webSocket.loop();

  int value = touchRead(TOUCH_PIN);

  // 🔍 DEBUG (VERY IMPORTANT)
  Serial.print("Touch Value: ");
  Serial.println(value);

  // ==========================
  // TOUCH DETECTED
  // ==========================
  if (value < threshold) {

    if (touchStartTime == 0) {
      touchStartTime = millis();
    }

    // LONG PRESS → RESET WIFI
    if (!longPressTriggered && (millis() - touchStartTime > 5000)) {
      longPressTriggered = true;

      Serial.println("Long Press → Reset WiFi");

      WiFiManager wm;
      wm.resetSettings();

      delay(1000);
      ESP.restart();
    }

  }
  else {

    // TOUCH RELEASED → TAP
    if (touchStartTime > 0 && !longPressTriggered) {

      currentLevel++;
      if (currentLevel >= 5) currentLevel = 0;

      setBrightness(currentLevel);

      char msg[5];
      sprintf(msg, "%d", currentLevel * 25);
      webSocket.sendTXT(msg);

      Serial.println("Tap detected");
    }

    // reset states
    touchStartTime = 0;
    longPressTriggered = false;
  }

  delay(50); // small stability delay
}