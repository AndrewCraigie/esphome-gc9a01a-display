# ESP32-S3 Development Board with 1.28" Round Touch LCD - Product Information

**Source:** [The Pi Hut Product Page](https://thepihut.com/products/esp32-s3-development-board-with-1-28-round-touch-lcd)

## Product Overview

This ESP32-S3 development board features an **Xtensa® 32-bit LX7 dual-core processor** that clocks up to **240MHz** and includes a **1.28" capacitive touch display** with sharp **240 x 240 resolution** supporting **65K colours** for vibrant and colourful visuals.

## Key Features

### Processor & Memory
- **CPU:** Xtensa® 32-bit LX7 dual-core processor
- **Clock Speed:** Up to 240MHz
- **SRAM:** 512KB
- **ROM:** 384KB
- **PSRAM:** 2MB onboard
- **Flash Memory:** 16MB external

### Connectivity
- **Wi-Fi:** 2.4GHz (802.11 b/g/n)
- **Bluetooth:** Bluetooth® 5 (LE)
- **Antenna:** Built-in antenna
- **Integration:** Easy integration into IoT ecosystems

### Display Specifications
- **Size:** 1.28" round display
- **Resolution:** 240(H)RGB x 240(V) pixels
- **Display Panel:** IPS
- **Display Chip:** GC9A01A
- **Display Interface:** SPI
- **Physical Size:** Φ32.4mm
- **Pixel Size:** 0.135 (H) x 0.135 (V) mm
- **Colors:** 65K color support

### Touch Controller
- **Touch Chip:** CST816S
- **Type:** Capacitive touch
- **Interface:** I2C

### Motion Sensor (IMU)
- **Sensor:** QMI8658 6-axis IMU
- **Components:** 3-axis accelerometer + 3-axis gyroscope
- **Applications:** Motion and gesture detection projects

#### Accelerometer Parameters
- **Resolution:** 16-bit
- **Range Options:** ±2, ±4, ±8, ±16g

#### Gyroscope Parameters
- **Resolution:** 16-bit
- **Range Options:** ±16, ±32, ±64, ±128, ±256, ±512, ±1024, ±2048°/sec

### Power & Connectivity
- **Connector:** USB-C
- **Battery Support:** Li-ion battery recharge manager
- **Portable Use:** Suitable for portable projects

## Important Notes

### Battery Compatibility Warning
⚠️ **Battery Connector:** This board uses a **tiny MX1.25 connector** for LiPo power. Most LiPo batteries on the market (including Pi Hut's) use a larger JST fitting. 

## Technical Specifications Summary

| **Component** | **Specification** |
|---------------|------------------|
| **Processor** | ESP32-S3 Xtensa® 32-bit LX7 dual-core, 240MHz |
| **Memory** | 512KB SRAM, 384KB ROM, 2MB PSRAM, 16MB Flash |
| **Display** | 1.28" round, 240×240, IPS, GC9A01A controller |
| **Touch** | CST816S capacitive touch via I2C |
| **IMU** | QMI8658 6-axis (3-axis accel + 3-axis gyro) |
| **Connectivity** | Wi-Fi 2.4GHz, Bluetooth 5 LE, built-in antenna |
| **Power** | USB-C, Li-ion battery support, MX1.25 connector |
| **Interfaces** | SPI (display), I2C (touch), USB-C |

## Applications

- IoT projects with visual feedback
- Wearable devices and smartwatches
- Motion and gesture detection systems
- Portable embedded systems
- Touch-based control interfaces
- Data logging with visual display
- Battery-powered sensor nodes

## Compatibility

- **Arduino IDE:** Supported with ESP32-S3 board package
- **ESP-IDF:** Native Espressif development framework
- **MicroPython:** Compatible
- **CircuitPython:** Compatible