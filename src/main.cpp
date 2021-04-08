#include <Arduino.h>
#include <FirebaseESP32.h>
#include <M5Stack.h>
#include <WiFi.h>
#include "secret.h"

FirebaseData firebaseData;

int count = 0;

bool buttonProcess() {
  count++;
  M5.Lcd.fillScreen(BLACK);

  if (Firebase.setInt(firebaseData, "/count/", count)) {
    //Success
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print("Set data success");
  } else {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Error in setInt, ");
    M5.Lcd.print(firebaseData.errorReason());
  }

  M5.Lcd.setCursor(0, 20);
  M5.Lcd.print("count: " + String(count));
  return 0;
}

void setup() {

  M5.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int _cursorX = 0;
  M5.Lcd.setTextFont(4);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 0);

  // WiFiに接続
  M5.Lcd.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    M5.Lcd.setCursor(0 + 5 * _cursorX, 30);
    M5.Lcd.print(".");
    delay(300);
    _cursorX++;
    if (_cursorX > 320) {
      _cursorX = 0;
    }
  }

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Connected with IP:");
  M5.Lcd.print(WiFi.localIP());
  delay(1000);

  // Firebase関連
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (M5.BtnC.wasPressed()) {
    buttonProcess();
  }

  M5.update();
}