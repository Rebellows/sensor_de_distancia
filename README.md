# Barreira de Proteção

Proximity alert system using GPS + LoRa on two ESP32-C3 boards. Built for the Programação de Periféricos course at PUCRS.

## Concept

Two boards communicate over LoRa radio. The **aggressor** board continuously transmits its GPS coordinates. The **victim** board receives them, calculates the distance to the aggressor, and triggers a Telegram alert if the aggressor enters a defined radius.

```
[Aggressor] ---(LoRa 433 MHz)---> [Victim]
  GPS fix                          GPS fix
  sends lat,lng                    calculates distance
                                   triggers alert if < 2m
```

## Hardware

Both boards use the same components:

| Component | Model |
|---|---|
| Microcontroller | ESP32-C3 (esp32-c3-devkitm-1) |
| GPS module | NEO-6M |
| LoRa transceiver | SX1278 (Ra-02, 433 MHz) |

## Wiring

### GPS NEO-6M → ESP32-C3

| GPS Pin | ESP32-C3 Pin |
|---|---|
| VCC | 3V3 |
| GND | GND |
| TX | GPIO20 (RX) |
| RX | GPIO21 (TX) |

### LoRa SX1278 → ESP32-C3

| SX1278 Pin | ESP32-C3 Pin |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SCK | GPIO4 |
| MISO | GPIO5 |
| MOSI | GPIO6 |
| NSS (CS) | GPIO7 |
| RST | GPIO3 |
| DIO0 | GPIO2 |

> **Important:** The SX1278 is strictly 3.3V. Connecting VCC to 5V will damage the module.

## Project Structure

```
project/
├── aggressor/        # Sender firmware
│   └── src/
│       └── main.cpp
└── victim/           # Receiver firmware
    └── src/
        └── main.cpp
```

## Firmware

### Aggressor (Sender)

- Reads GPS coordinates from the NEO-6M via UART
- Once a valid GPS fix is obtained, transmits `lat,lng` as a comma-separated string over LoRa every 3 seconds
- Example packet: `-30.060082,-51.174248`

### Victim (Receiver)

- Reads its own GPS coordinates from the NEO-6M via UART
- Listens for incoming LoRa packets continuously
- Parses received `lat,lng` strings and calculates distance to the aggressor using `TinyGPSPlus::distanceBetween()`
- If distance is within `ALERT_RADIUS_M` (default: 100 meters), triggers a Telegram alert via Wi-Fi

## LoRa Configuration

Both boards must use identical radio settings:

| Parameter | Value |
|---|---|
| Frequency | 433 MHz |
| Spreading Factor | 7 |
| Bandwidth | 125 kHz |
| Coding Rate | 4/5 |
| TX Power | 17 dBm (aggressor only) |

## Dependencies

Add to `platformio.ini` under `lib_deps`:

```ini
lib_deps =
    mikalhart/TinyGPSPlus
    sandeepmistry/LoRa
```

## platformio.ini (both boards)

```ini
[env:esp32-c3-devkitm-1]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
monitor_speed = 115200
build_flags =
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
lib_deps =
    mikalhart/TinyGPSPlus
    sandeepmistry/LoRa
```

## Alert Radius

The alert threshold is defined in the victim's `main.cpp`:

```cpp
#define ALERT_RADIUS_M 2  // meters
```

Change this value to adjust how close the aggressor must be before an alert fires.

## Known Quirks

- The ESP32-C3 sometimes requires manual bootloader mode to flash: hold **BOOT**, press **RESET**, release **BOOT**, then upload
- USB CDC serial output requires the build flags above — without them the serial monitor shows nothing
- Folder names with spaces in the PlatformIO project path cause linker errors — keep paths clean
- GPS acquires a fix faster outdoors; indoors it may take several minutes or fail entirely# sensor_de_distancia
ESP32C3 project using GPS and LoRa to communicate between two interfaces finding the distance between both, to simulate aggressor and victim
