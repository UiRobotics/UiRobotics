#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C
int delayTime = 1000;
bool recieving = false;

void setup() {
    Serial.begin(9600);
    bool status = bme.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
}

String rec = "";
void loop() { 
    while (!recieving){
      rec = Serial.readString();
      Serial.print(rec);
      if (rec == "ready") {
        recieving = true;
      }
      delay(delayTime);
    }
    float temp = bme.readTemperature();
    float pressure = bme.readPressure() / 100.0F;
    float humidity = bme.readHumidity();
    if (isnan(temp)) {
      temp = 0;
    }
    if (isnan(pressure)) {
      pressure = 0;
    }
    if (isnan(humidity)) {
      humidity = 0;
    }
    String message = "Nuc:temp-" + String(temp) + ":pressure-" + String(pressure) + ":humidity-" + String(humidity) + ";";
    Serial.println(message);
    delay(delayTime);
}
