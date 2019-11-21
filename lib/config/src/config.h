#ifndef CONFIG_H
#define CONFIG_H
#include <WString.h>
#include <Arduino.h>
#include "SPIFFS.h"
#include <FS.h>
#include <ArduinoJson.h>

#define CONFIG_JSON_FILE 200

class Config{
private:
    String filePath;
    String jsonData;
    using JsonDocument = StaticJsonDocument<CONFIG_JSON_FILE>;
    JsonDocument doc;

public:
    Config(String path);
    bool readData();
    JsonDocument get() { return doc; };
};


#endif