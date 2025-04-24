#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic;
bool deviceConnected = false;
std::string rxValue;

// UUIDs for the UART service
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.print("Received: ");
      Serial.println(rxValue.c_str());

      if (rxValue == "leds") {
        std::string reply = "YEAH\n";
        Serial.println("Sending Reply: YEAH");
        pTxCharacteristic->setValue(reply);
        pTxCharacteristic->notify();
      }
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Init BLE
  BLEDevice::init("ESP32test");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create TX characteristic
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
                      );
  pTxCharacteristic->addDescriptor(new BLE2902());

  // Create RX characteristic
  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
                                          CHARACTERISTIC_UUID_RX,
                                          BLECharacteristic::PROPERTY_WRITE
                                        );
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Waiting for a client connection...");
}

void loop() {
  // You could add logic here for when deviceConnected == true
  delay(100);
}
