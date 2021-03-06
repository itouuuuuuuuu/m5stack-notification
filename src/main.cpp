#include <Arduino.h>
#include <FirebaseESP32.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <M5Stack.h>
#include <WiFi.h>
#include "secret.h"

FirebaseData firebaseData;
WiFiServer server(80);

String ssid = "M5S_AP";
String password = "12345678";
String wifiSsid = "";
String wifiPassword = "";
int wifiStatus = WiFi.status();
int count = 0;

String getSsid(String path) {
  int start = path.indexOf("?ssid=");
  int end = path.indexOf("&");
  return path.substring(start + 6, end);
}

String getPassword(String path) {
  int start = path.indexOf("&password=");
  int end = path.length();
  return path.substring(start + 10, end);
}

bool buttonProcess() {
  if (wifiStatus != WL_CONNECTED) {
    return 0;
  }

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

  M5.Lcd.print("count: " + String(count));
  return 0;
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);

  //// for random password
  // randomSeed(analogRead(0));
  // for (int i = 0; i < 8; i++) {
  //   password += random(10);
  // }

  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid.c_str(), password.c_str());

  M5.Lcd.println("Please connect access point.");
  M5.Lcd.println("SSID:" + ssid);
  M5.Lcd.println("PASS:" + password);
  M5.Lcd.println();
  M5.Lcd.println("And access bleow url!");
  M5.Lcd.println("http://192.168.4.1/");
  M5.Lcd.qrcode("http://192.168.4.1/", 80, 65, 160, 6);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  server.begin();
  Serial.println("Web server started");

  MDNS.addService("http", "tcp", 80);
}

void loop() {
  if (wifiSsid.length() > 0 && wifiPassword.length() > 0) {
    if (wifiStatus != WL_CONNECTED) {
      M5.Lcd.setCursor(0, 0, 1);
      M5.Lcd.fillScreen(BLACK);

      Serial.println("SSID: " + wifiSsid);
      Serial.println("Pass: " + wifiPassword);

      delay(10);
      WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
      while (wifiStatus != WL_CONNECTED) {
        delay(500);
        wifiStatus = WiFi.status();
        Serial.print(".");
        M5.Lcd.setCursor(0, 0, 1);
        M5.Lcd.println("WiFi connecting...");
      }

      M5.Lcd.setCursor(0, 0, 1);
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextSize(3);
      M5.Lcd.println("Connected!");
      M5.Lcd.setTextSize(1);
      M5.Lcd.println("Please connect internet and access below url!");
      M5.Lcd.println("https://m5stack-notification.web.app/");
      M5.Lcd.qrcode("https://m5stack-notification.web.app/", 80, 65, 160, 6);

      // // Firebase??????
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
      // Firebase.reconnectWiFi(true);

      delay(500);

      Serial.print("WiFi connected\r\nIP address: ");
      Serial.println(WiFi.localIP());
    }
  } else {
    WiFiClient client = server.available();
    if (!client) {
      return;
    }

    Serial.println("");
    Serial.println("New client");
    if (client) {
      Serial.println("new client");

      while (client.connected()) {
        if (client.available()) {
          String req = client.readStringUntil('\r');
          Serial.print("Request: ");
          Serial.println(req);

          int addr_start = req.indexOf(' ');
          int addr_end = req.indexOf(' ', addr_start + 1);
          if (addr_start == -1 || addr_end == -1) {
            Serial.print("Invalid request: ");
            Serial.println(req);
            return;
          }

          String path = req.substring(addr_start + 1, addr_end);
          Serial.print("Path: ");
          Serial.println(path);

          String s = "";
          if (path == "/") {
            IPAddress ip = client.remoteIP(); // ????????????????????????IP????????????
            String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
            s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            s += "<!DOCTYPE html>";
            s += "<html lang=\"ja\">";
            s += "  <head>";
            s += "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>";
            s += "    <style>";
            s += "      body {";
            s += "        font-size: 48px;";
            s += "        margin: 0;";
            s += "        background-color: #f2f2f2;";
            s += "        font-family: 'Arial',YuGothic,'Yu Gothic','Hiragino Kaku Gothic ProN','?????????????????? ProN W3','????????????', Meiryo,'?????? ????????????',sans-serif;";
            s += "      }";
            s += "      header {";
            s += "        width: 100%;";
            s += "        height: 120px;";
            s += "        line-height: 120px;";
            s += "        background-color: #F1B514;";
            s += "        text-align: center;";
            s += "        color: #f2f2f2;";
            s += "        font-weight: bold;";
            s += "      }";
            s += "      main {";
            s += "        padding-left: 24px;";
            s += "        padding-right: 24px;";
            s += "      }";
            s += "      input {";
            s += "        width: 100%;";
            s += "        height: 80px;";
            s += "        border: none;";
            s += "        font-size: 48px;";
            s += "        padding: 16px;";
            s += "        margin-top: 16px;";
            s += "      }";
            s += "      button {";
            s += "        width: 100%;";
            s += "        height: 120px;";
            s += "        background-color: #254DEA;";
            s += "        font-size: 48px;";
            s += "        color: #f2f2f2;";
            s += "        border: none;";
            s += "        border-radius: 60px;";
            s += "        margin-top: 40px;";
            s += "        font-weight: bold;";
            s += "      }";
            s += "    </style>";
            s += "  </head>";
            s += "  <body>";
            s += "    <header>M5StackNotification WiFi??????</header>";
            s += "    <main>";
            s += "      <form method=\"GET\" action=\"/connect\">";
            s += "        <input type=\"text\" name=\"ssid\" placeholder=\"SSID\" value=\"itopixel\" />";
            s += "        <input type=\"password\" name=\"password\" placeholder=\"Password\"  value=\"12345678\" />";
            s += "        <button type=\"submit\">????????????</button>";
            s += "      </form>";
            s += "    </main>";
            s += "  </body>";
            s += "</html>";
            Serial.println("Response 200");
          } else if (path.startsWith("/connect")) {
            Serial.println("send to /connect");
            s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            s += "<!DOCTYPE html>";
            s += "<html lang=\"ja\">";
            s += "  <head>";
            s += "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>";
            s += "    <style>";
            s += "      body {";
            s += "        font-size: 48px;";
            s += "        margin: 0;";
            s += "        background-color: #f2f2f2;";
            s += "        font-family: 'Arial',YuGothic,'Yu Gothic','Hiragino Kaku Gothic ProN','?????????????????? ProN W3','????????????', Meiryo,'?????? ????????????',sans-serif;";
            s += "      }";
            s += "      header {";
            s += "        width: 100%;";
            s += "        height: 120px;";
            s += "        line-height: 120px;";
            s += "        background-color: #F1B514;";
            s += "        text-align: center;";
            s += "        color: #f2f2f2;";
            s += "        font-weight: bold;";
            s += "      }";
            s += "      main {";
            s += "        padding-left: 24px;";
            s += "        padding-right: 24px;";
            s += "        text-align: center;";
            s += "      }";
            s += "      input {";
            s += "        width: 100%;";
            s += "        height: 80px;";
            s += "        border: none;";
            s += "        font-size: 48px;";
            s += "        padding: 16px;";
            s += "        margin-top: 16px;";
            s += "      }";
            s += "      button {";
            s += "        width: 100%;";
            s += "        height: 120px;";
            s += "        background-color: #254DEA;";
            s += "        font-size: 48px;";
            s += "        color: #f2f2f2;";
            s += "        border: none;";
            s += "        border-radius: 60px;";
            s += "        margin-top: 40px;";
            s += "        font-weight: bold;";
            s += "      }";
            s += "    </style>";
            s += "  </head>";
            s += "  <body>";
            s += "    <header>M5StackNotification WiFi??????</header>";
            s += "    <main>";
            s += "      <h4>WiFi????????????????????????...</h4>";
            s += "      <h6>???????????????Connected?????????????????????????????????????????????</h6>";
            s += "    </main>";
            s += "  </body>";
            s += "</html>";

            wifiSsid = getSsid(path);
            wifiPassword = getPassword(path);

            Serial.println("SSID: " + wifiSsid);
            Serial.println("Pass: " + wifiPassword);
            Serial.println("Response 200");
          } else {
            s = "HTTP/1.1 404 Not Found\r\n\r\n";
            Serial.println("Sending 404");
          }
          client.print(s);
          client.flush();
          client.stop();
        }
      }
    }
    Serial.println("Done with client");
  }

  if (M5.BtnC.wasPressed()) {
    buttonProcess();
  }

  M5.update();

  // delay(100);
}