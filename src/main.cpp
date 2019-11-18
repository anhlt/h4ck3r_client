#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <DallasTemperature.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <WString.h>
#include <WiFi.h>
#include <Wire.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;
const char *ssid = "dhr01-b6373d-g";
const char *password = "07798419e1301";

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.printstr(&(value[0]));
    if (value.length() > 0) {
      Serial.print("\r\nNew value: ");
      for (int i = 0; i < value.length(); i++)
        Serial.print(value[i]);
      Serial.println();
    }
  }
};

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.autoscroll();

  Serial.println("Connected to the WiFi network");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  Serial.println("Connected to the WiFi network");

  BLEDevice::init("ESP32 BLE example2");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {

  // if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection
  // status

  //   Serial.println(WiFi.localIP());
  //   HTTPClient http;

  //   http.begin("https://httpbin.org/post"); //Specify the URL int httpCode =
  //   http.POST("{id:1}");                                        //Make the
  //   request

  //   if (httpCode > 0) { //Check for the returning code

  //       String payload = http.getString();
  //       Serial.println(httpCode);
  //       Serial.println(payload);
  //     }

  //   else {
  //     Serial.println("Error on HTTP request");
  //   }

  //   http.end(); //Free the resources
  // }

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.println(temperatureC);
  lcd.setCursor(0, 0);
  lcd.print(temperatureC);
  lcd.setCursor(0, 1);
  String localIp = WiFi.localIP().toString();
  lcd.printstr(localIp.c_str());
  delay(1000);
}