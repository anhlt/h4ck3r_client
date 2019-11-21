#include "config.h"

Config::Config(String path) {
    Serial.println("==============");
    filePath = path;
    Serial.println(filePath);
    Serial.println("==============");
}

bool Config::readData() {
    Serial.println("readData");

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }
    File configFile = SPIFFS.open(filePath);
    if (!configFile) {
        Serial.println("Failed to open configFile");
        return false;
    }
    while (configFile.available()) {
        jsonData += char(configFile.read());
    }

    deserializeJson(doc, jsonData);

    configFile.close();
    SPIFFS.end();
    return true;
}