#include <Wire.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C


unsigned long delayTime;

void setup() {
  Serial.begin(9600);
  while (!Serial);   // time to get serial running
  //Serial.println(F("BME280 test"));

  unsigned status;

  // default settings
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.println("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    while (1) delay(10);
  }
  delayTime = 1000;
  delay(delayTime * 5);
}


void loop() {
  float temperature, pressure, humidity;
  BLEDevice::init("Sensor_1");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setValue("test sensora 1");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  while (1) {
    printValues(); //for debug
    pAdvertising->stop(); //ELOMELO
    temperature = bme.readTemperature();
    pressure=bme.readPressure() / 100.0F;
    humidity=bme.readHumidity();
    char buf2[24];
    snprintf(buf2, 24, "%.2f, %.2f, %.2f\r\n", temperature, pressure, humidity);
    pCharacteristic->setValue(buf2);
    pCharacteristic->notify();
    BLEDevice::startAdvertising(); //ELOMELO
    delay(delayTime);
  }
  delay(delayTime);
}

void printValues() {
  Serial.print(bme.readTemperature());
  Serial.print(", ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.print(", ");
  Serial.print(bme.readHumidity());
  Serial.println();
}
