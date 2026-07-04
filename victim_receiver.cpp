#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>

#define GPS_RX 20
#define GPS_TX 21

#define LORA_SCK  4
#define LORA_MISO 5
#define LORA_MOSI 6
#define LORA_CS   7
#define LORA_RST  3
#define LORA_DIO0 2
#define LORA_FREQ 433E6

#define ALERT_RADIUS_M 100.0  

HardwareSerial GPSserial(1);
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Vitima (Receiver) ===");

  GPSserial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("GPS initialized.");

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init FAILED!");
    while (true);
  }
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  Serial.println("LoRa initialized.");
}

void loop() {
  while (GPSserial.available()) {
    gps.encode(GPSserial.read());
  }

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    int commaIndex = received.indexOf(',');
    if (commaIndex != -1) {
      double aggressorLat = received.substring(0, commaIndex).toDouble();
      double aggressorLng = received.substring(commaIndex + 1).toDouble();

      Serial.print("Aggressor position: ");
      Serial.print(aggressorLat, 6);
      Serial.print(", ");
      Serial.println(aggressorLng, 6);

      if (gps.location.isValid()) {
        double distance = TinyGPSPlus::distanceBetween(
          gps.location.lat(), gps.location.lng(),
          aggressorLat, aggressorLng
        );

        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" m");

        if (distance <= ALERT_RADIUS_M) {
          Serial.println("!!! ALERT: Aggressor within radius !!!");
        }
      } else {
        Serial.println("Own GPS not valid yet, cannot calculate distance.");
      }
    }
  }

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 2000) {
    lastPrint = millis();
    Serial.println("-----------------------------");
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
    if (gps.location.isValid()) {
      Serial.print("Latitude:  ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
    } else {
      Serial.println("Location: NOT valid yet");
    }
  }
}
