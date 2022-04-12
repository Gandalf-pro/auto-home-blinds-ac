#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <config.h>
#include <pins.h>

#include <FSDataHandler.hpp>
#include <Features/AcFeature.hpp>
#include <Features/BlindsFeature.hpp>
#include <Features/LightLevelFeature.hpp>
#include <General.hpp>
#include <MqttHandler.hpp>
#include <setup.hpp>

// Objects
home::Device* device;
home::AcFeature* acf;
home::BlindsFeature* bf;
home::LightLevelFeature* lightLevelFeature;
FSDataHandler* dataHandler;
MqttHandler* mqttHandler;

void setupObjects() {
    // Objects
    dataHandler = new FSDataHandler(device, "BlindsAc", "ozgur");
    dataHandler->readData();
    device = new home::Device(dataHandler->getRoomName(), dataHandler->getDeviceName());
    // Ac
    acf = new home::AcFeature(*device, "ac", Pins::ir);
    // Blinds
    bf = new home::BlindsFeature(*device, "blinds", Pins::relayUp, Pins::relayDown, Pins::switchButton, Pins::buttonUp, Pins::buttonDown);
    // Light level
    lightLevelFeature = new home::LightLevelFeature(*device, "lightLevel", Pins::lightSensor);

    mqttHandler = new MqttHandler(dataHandler, device);
    // Add the fatures
    device->addFeature(acf);
    device->addFeature(bf);
    device->addFeature(lightLevelFeature);
}

void setup() {
    Serial.begin(9600);
    Setup::setup_wifi();
    Setup::setupOta(HOSTNAME, OTA_PASS);
    // pin setup
    Setup::pinSetup();
    // Littlefs
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
    }
    String mqttServerIp = Setup::dicoverMqttServer(false);
    // If we got an adress
    if (mqttServerIp.equals("0")) {
        ESP.reset();
    }
    setupObjects();

    // Mqtt setup
    mqttHandler->mqttSetup(mqttServerIp);

    // Final device fatures setup
    device->setup();
}

void loop() {
    ArduinoOTA.handle();
    mqttHandler->mqttLoop();
    device->loop();
}

void gotMessage(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    DynamicJsonDocument doc(255);
    // deserialize the data
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        Serial.printf("Deserialization failed reason:%s\n", err.c_str());
        return;
    }

    // Execute it
    device->execute(doc);

    // send all the data if asked
    if (strcmp(topic, "all/data") == 0 && doc.containsKey("data")) {
        if (doc["data"]) {
            // Send all values
            mqttHandler->publish("data", device->getData().c_str());
        }
    }
}