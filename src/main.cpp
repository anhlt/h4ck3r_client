#include <Arduino.h>
#include <DallasTemperature.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <WString.h>
#include <WiFi.h>
#include <Wire.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "config.h"
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <WiFiManager.h>
#include <OneButton.h>
#include "rgbled.h"
#include "EOTAUpdate.h"

const char * const   VERSION_STRING = "1.0";
const unsigned short VERSION_NUMBER = 2;
const char * const   UPDATE_URL     = "http://github.com/anhlt/h4ck3r_client/releases/latest/download/release_info.txt";
EOTAUpdate updater(UPDATE_URL, VERSION_NUMBER);
// Config

const String path = "/config.json";
// WebSocket
WebSocketsClient webSocket;
#define USE_SERIAL Serial

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
  const uint8_t* src = (const uint8_t*) mem;
  USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for (uint32_t i = 0; i < len; i++) {
    if (i % cols == 0) {
      USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  USE_SERIAL.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

    // send message to server when Connected
    webSocket.sendTXT("Connected");
    break;
  case WStype_TEXT:
    USE_SERIAL.printf("[WSc] get text: %s\n", payload);

    // send message to server
    // webSocket.sendTXT("message here");
    break;
  case WStype_BIN:
    USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);

    // send data to server
    // webSocket.sendBIN(payload, length);
    break;
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }

}


// GPIO where the DS18B20 is connected to
const int oneWireBus = A13;

//

//ledpin
const int ledR = A18;
const int ledG = A5;
const int ledB = A4;

RgbLed led = RgbLed(ledR, ledG, ledB);

// Button

OneButton button(A6, true);

void doubleclick() {
  led.toggle();

  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  Serial.println("Clicked");
}; // doubleclick

// Temprature Setup
// Setup a oneWire instance to communicate with any OneWire devices

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);


// WIFIMANAGER

#include <Ticker.h>
Ticker ticker;

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, []{
    led.toggle();
  });
}



int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);


void setup() {
  Serial.begin(115200);
  button.attachClick(doubleclick);
  // start ticker with 0.5 because we start in AP mode and try to connect
  led.turnOn(RED);
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.noAutoscroll();
  auto config = Config(path);
  config.readData();
  auto configData = config.get();

  Serial.println("Connected to the WiFi network");


  Serial.println(WiFi.localIP());
  Serial.println("Connected to the WiFi network");


  webSocket.begin("192.168.10.108", 9000, "/socket");
  webSocket.onEvent(webSocketEvent);



}

void loop() {
  updater.CheckAndUpdate();
  button.tick();
  webSocket.loop();


  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  // webSocket.sendTXT("uint8_t *payload");
  lcd.setCursor(0, 0);
  lcd.print(temperatureC);
  lcd.setCursor(0, 1);
  String localIp = WiFi.localIP().toString();
  lcd.printstr(localIp.c_str());
}