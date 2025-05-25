# ESP32-S3 Touch LCD 1.28" - Pinout and Schematic Reference

**PCB:** 1.0mm thickness  
**Device:** ESP32-S3-Touch-LCD-1.28  
**Source:** Official device schematic

## ESP32-S3 Main Processor (U1)

### Power Pins
| Pin | Function | Connection |
|-----|----------|------------|
| VDD3P3_RTC | 3.3V RTC Power | 3V3 |
| VDD3P3_CPU | 3.3V CPU Power | 3V3 |
| VDD_SPI | SPI Power | 3V3 |
| VDDA | Analog Power | 3V3 |
| VDD3P3 | 3.3V Main Power | 3V3 |
| GND | Ground | Multiple ground connections |

### GPIO Connections

#### Display Interface (SPI)
| GPIO Pin | Function | Connected To | Description |
|----------|----------|--------------|-------------|
| GPIO8 | LCD_DC | GC9A01A Display | Data/Command control |
| GPIO9 | LCD_CS | GC9A01A Display | Chip Select |
| GPIO10 | LCD_CLK | GC9A01A Display | SPI Clock |
| GPIO11 | LCD_MOSI | GC9A01A Display | SPI Data Out |
| GPIO12 | LCD_MISO | GC9A01A Display | SPI Data In |
| GPIO14 | LCD_RST | GC9A01A Display | Reset |
| GPIO2 | LCD_BL | Display Backlight | Backlight Control |

#### Touch Interface (I2C)
| GPIO Pin | Function | Connected To | Description |
|----------|----------|--------------|-------------|
| GPIO6 | I2C_SDA | CST816S Touch | I2C Data |
| GPIO7 | I2C_SCL | CST816S Touch | I2C Clock |
| GPIO5 | Touch_INT | CST816S Touch | Touch Interrupt |
| GPIO13 | Touch_RST | CST816S Touch | Touch Reset |

#### IMU Interface (I2C - Shared)
| GPIO Pin | Function | Connected To | Description |
|----------|----------|--------------|-------------|
| GPIO6 | I2C_SDA | QMI8658 IMU | I2C Data (shared with touch) |
| GPIO7 | I2C_SCL | QMI8658 IMU | I2C Clock (shared with touch) |

#### Power Management
| GPIO Pin | Function | Connected To | Description |
|----------|----------|--------------|-------------|
| GPIO1 | BAT_ADC | Battery Voltage | Battery voltage monitoring |

#### Crystal and Boot
| GPIO Pin | Function | Connected To | Description |
|----------|----------|--------------|-------------|
| GPIO0 | BOOT | Boot button | Boot mode selection |
| XTAL_32K_P | Crystal 32kHz | XTAL circuit | 32kHz crystal positive |
| XTAL_32K_N | Crystal 32kHz | XTAL circuit | 32kHz crystal negative |

### USB Interface
| Pin | Function | Connected To | Description |
|-----|----------|--------------|-------------|
| GPIO19 | USB_D- | USB-C connector | USB Data Negative |
| GPIO20 | USB_D+ | USB-C connector | USB Data Positive |

## Display Controller (GC9A01A)

### SPI Interface
| Pin | Function | ESP32-S3 GPIO | Description |
|-----|----------|---------------|-------------|
| SDA | SPI Data | GPIO11 (MOSI) | Serial Data Input |
| SCL | SPI Clock | GPIO10 | Serial Clock |
| CS | Chip Select | GPIO9 | Chip Select (Active Low) |
| DC | Data/Command | GPIO8 | Data/Command Control |
| RST | Reset | GPIO14 | Reset (Active Low) |

### Power and Control
| Pin | Function | Connection | Description |
|-----|----------|------------|-------------|
| VDD | Power | 3V3 | 3.3V Power Supply |
| GND | Ground | GND | Ground |
| BLK | Backlight | GPIO2 | Backlight Control |

## Touch Controller (CST816S)

### I2C Interface
| Pin | Function | ESP32-S3 GPIO | Description |
|-----|----------|---------------|-------------|
| SDA | I2C Data | GPIO6 | I2C Serial Data |
| SCL | I2C Clock | GPIO7 | I2C Serial Clock |
| RST | Reset | GPIO13 | Reset (Active Low) |
| INT | Interrupt | GPIO5 | Touch Interrupt Output |

### Power
| Pin | Function | Connection | Description |
|-----|----------|------------|-------------|
| VDD | Power | 3V3 | 3.3V Power Supply |
| GND | Ground | GND | Ground |

## IMU Sensor (QMI8658)

### I2C Interface (Shared with Touch)
| Pin | Function | ESP32-S3 GPIO | Description |
|-----|----------|---------------|-------------|
| SDA | I2C Data | GPIO6 | I2C Serial Data |
| SCL | I2C Clock | GPIO7 | I2C Serial Clock |
| INT1 | Interrupt 1 | Not connected | Interrupt Output 1 |
| INT2 | Interrupt 2 | Not connected | Interrupt Output 2 |

### Power
| Pin | Function | Connection | Description |
|-----|----------|------------|-------------|
| VDD | Power | 3V3 | 3.3V Power Supply |
| GND | Ground | GND | Ground |

## Flash Memory (External)

### SPI Interface
| Pin | Function | ESP32-S3 Pin | Description |
|-----|----------|--------------|-------------|
| CLK | SPI Clock | SPICS1 | SPI Clock |
| DI | Data In | SPID | SPI Data Input |
| DO | Data Out | SPIQ | SPI Data Output |
| CS | Chip Select | SPICS0 | Chip Select |

## Power Management

### Battery Management
| Component | Function | Connection | Description |
|-----------|----------|------------|-------------|
| BAT+ | Battery Positive | MX1.25 Connector | LiPo Battery Positive |
| BAT- | Battery Negative | MX1.25 Connector | LiPo Battery Negative |
| Battery ADC | Voltage Monitor | GPIO1 | Battery voltage monitoring |

### Power Supply
| Rail | Voltage | Source | Description |
|------|---------|--------|-------------|
| 3V3 | 3.3V | USB-C / Battery | Main 3.3V rail |
| VDD5V | 5V | USB-C | USB 5V input |
| VBUS | USB Power | USB-C | USB power detection |

## USB-C Connector Pinout

| Pin | Function | Connection | Description |
|-----|----------|------------|-------------|
| 1 | GND | GND | Ground |
| 2 | SBU2 | Not connected | Sideband Use 2 |
| 3 | CC2 | CC detection | Configuration Channel 2 |
| 4 | D+ | GPIO20 | USB Data Positive |
| 5 | D- | GPIO19 | USB Data Negative |
| 6 | VBUS | Power circuit | USB Power |

## I2C Device Addresses

| Device | I2C Address | Interface Pins |
|--------|-------------|----------------|
| CST816S Touch | 0x15 | SDA=GPIO6, SCL=GPIO7 |
| QMI8658 IMU | 0x6B | SDA=GPIO6, SCL=GPIO7 |

## Key Design Notes

### Shared Interfaces
- **I2C Bus:** Touch controller (CST816S) and IMU (QMI8658) share the same I2C bus on GPIO6/GPIO7
- **SPI Bus:** Display uses dedicated SPI pins, Flash uses internal SPI bus

### Power Considerations
- **Battery Connector:** Uses MX1.25 connector (not standard JST)
- **Power Rails:** Single 3.3V rail powers all peripherals
- **Battery Monitoring:** GPIO1 provides battery voltage feedback

### Reset and Boot
- **Boot Mode:** GPIO0 connected to boot button for programming mode
- **System Reset:** Multiple reset signals for different components
- **Crystal:** 32kHz crystal for RTC functionality

## Pin Usage Summary

### Used GPIO Pins
- GPIO0: Boot button
- GPIO1: Battery ADC
- GPIO2: LCD Backlight
- GPIO5: Touch Interrupt
- GPIO6: I2C SDA (Touch + IMU)
- GPIO7: I2C SCL (Touch + IMU)
- GPIO8: LCD Data/Command
- GPIO9: LCD Chip Select
- GPIO10: LCD SPI Clock
- GPIO11: LCD SPI MOSI
- GPIO12: LCD SPI MISO
- GPIO13: Touch Reset
- GPIO14: LCD Reset
- GPIO19: USB D-
- GPIO20: USB D+

### Available GPIO Pins
The schematic shows additional GPIO pins available on the ESP32-S3 that could be used for expansion, though they may not be broken out to external connectors on this compact board design.