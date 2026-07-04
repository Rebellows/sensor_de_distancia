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

HardwareSerial GPSserial(1);
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Agressor (Sender) ===");

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
  LoRa.setTxPower(17);
  Serial.println("LoRa initialized.");
}

void loop() {
  while (GPSserial.available()) {
    gps.encode(GPSserial.read());
  }

  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 3000) {
    lastSend = millis();

    if (gps.location.isValid()) {
      String payload = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);

      Serial.print("Sending: ");
      Serial.println(payload);

      LoRa.beginPacket();
      LoRa.print(payload);
      LoRa.endPacket();
    } else {
      Serial.println("No GPS fix yet, not sending.");
    }
  }
}
