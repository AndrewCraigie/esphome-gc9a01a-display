# ESPHome GC9A01A Touch LCD Project - Component References

**Project:** ESP32-S3 1.28" Round Touch LCD with GC9A01A Display Driver  
**Purpose:** Quick reference links to relevant ESPHome components and documentation

---

## 📚 General Documentation

### ESPHome Components Overview
- **Main Documentation:** [ESPHome Components](https://esphome.io/components/)
- **Raw Source:** [components/index.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/index.rst)

---

## ⚙️ Core System Components

### ESPHome Core Configuration
- **Documentation:** [ESPHome Core](https://esphome.io/components/esphome)
- **Raw Source:** [components/esphome.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/esphome.rst)
- **Purpose:** Basic device configuration, board selection, and core settings

### PSRAM Configuration
- **Documentation:** [PSRAM Component](https://esphome.io/components/psram)
- **Raw Source:** [components/psram.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/psram.rst)
- **Purpose:** Enable external PSRAM on ESP32-S3 for larger framebuffers

### External Components
- **Documentation:** [External Components](https://esphome.io/components/external_components)
- **Raw Source:** [components/external_components.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/external_components.rst)
- **Purpose:** Include custom components from external repositories

---

## 🔌 Communication Bus Components

### SPI Bus
- **Documentation:** [SPI Bus](https://esphome.io/components/spi)
- **Raw Source:** [components/spi.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/spi.rst)
- **Purpose:** Configure SPI bus for GC9A01A display communication
- **Project Use:** Display controller communication

### I2C Bus
- **Documentation:** [I2C Bus](https://esphome.io/components/i2c#i2c-bus)
- **Raw Source:** [components/i2c.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/i2c.rst)
- **Purpose:** Configure I2C bus for touch controller and IMU
- **Project Use:** CST816S touch controller, QMI8658 IMU

### UART Bus
- **Documentation:** [UART Bus](https://esphome.io/components/uart)
- **Raw Source:** [components/uart.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/uart.rst)
- **Purpose:** Serial communication interface
- **Project Use:** Debugging, optional external sensors

---

## 🖥️ Display Components

### Display Component (Base)
- **Documentation:** [Display Component](https://esphome.io/components/display/)
- **Raw Source:** [components/display/index.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/display/index.rst)
- **Purpose:** Base display functionality, drawing primitives, and display automation
- **Project Use:** Foundation for custom GC9A01A display driver

### ILI9xxx TFT LCD Series (Reference)
- **Documentation:** [ILI9xxx Display](https://esphome.io/components/display/ili9xxx)
- **Raw Source:** [components/display/ili9xxx.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/display/ili9xxx.rst)
- **Purpose:** Reference implementation for similar SPI TFT displays
- **Project Use:** Architecture reference for GC9A01A implementation

### Display Menu
- **Documentation:** [Display Menu](https://esphome.io/components/display_menu/)
- **Raw Source:** [components/display_menu/index.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/display_menu/index.rst)
- **Purpose:** Create interactive menus on displays
- **Project Use:** Touch-based menu systems

---

## 🖱️ Touch Interface Components

### Touchscreen Components (Base)
- **Documentation:** [Touchscreen Components](https://esphome.io/components/touchscreen/)
- **Raw Source:** [components/touchscreen/index.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/touchscreen/index.rst)
- **Purpose:** Base touchscreen functionality and touch event handling
- **Project Use:** Foundation for touch interaction

### CST816 Touch Screen Controller
- **Documentation:** [CST816 Touch Controller](https://esphome.io/components/touchscreen/cst816)
- **Raw Source:** [components/touchscreen/cst816.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/touchscreen/cst816.rst)
- **Purpose:** Specific driver for CST816S capacitive touch controller
- **Project Use:** **Direct hardware match** - CST816S on the ESP32-S3 board

---

## 🎨 Advanced Graphics Components

### LVGL Graphics
- **Documentation:** [LVGL Graphics](https://esphome.io/components/lvgl/)
- **Raw Source:** [components/lvgl/index.rst](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/lvgl/index.rst)
- **Purpose:** Advanced graphics library with widgets, animations, and themes
- **Project Use:** Enhanced UI capabilities for the round display

---

## 📋 Component Usage Priority

### **Essential Components** (Required for basic functionality)
1. **ESPHome Core** - Device configuration
2. **PSRAM** - Memory expansion for framebuffer
3. **SPI Bus** - Display communication
4. **I2C Bus** - Touch and IMU communication
5. **Display Component** - Base display functionality

### **Direct Hardware Components** (Specific to this hardware)
1. **CST816 Touch Controller** - **Exact hardware match**
2. **External Components** - For custom GC9A01A driver

### **Reference Components** (For implementation guidance)
1. **ILI9xxx Display** - Similar SPI display implementation
2. **Touchscreen Base** - Touch event handling patterns

### **Optional Enhancement Components**
1. **Display Menu** - Interactive menus
2. **LVGL Graphics** - Advanced graphics capabilities
3. **UART** - Additional communication/debugging

---

## 🔗 Quick Access Links

| Component Type | Primary Link | Backup Raw Link |
|----------------|--------------|-----------------|
| **Core Config** | [esphome.io](https://esphome.io/components/esphome) | [raw](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/esphome.rst) |
| **SPI Bus** | [esphome.io](https://esphome.io/components/spi) | [raw](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/spi.rst) |
| **I2C Bus** | [esphome.io](https://esphome.io/components/i2c#i2c-bus) | [raw](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/i2c.rst) |
| **Display Base** | [esphome.io](https://esphome.io/components/display/) | [raw](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/display/index.rst) |
| **CST816 Touch** | [esphome.io](https://esphome.io/components/touchscreen/cst816) | [raw](https://raw.githubusercontent.com/esphome/esphome-docs/refs/heads/current/components/touchscreen/cst816.rst) |

---

## 📝 Notes

### URL Types Explained
- **Documentation URLs** (`esphome.io/components/...`): User-friendly web documentation with examples
- **Raw Source URLs** (`raw.githubusercontent.com/...`): Direct access to RST source files for offline reference

### Component Dependencies
- Most display components depend on the base **Display Component**
- Touch components require **I2C Bus** configuration
- **LVGL** requires significant memory - **PSRAM** component is essential
- **External Components** needed for custom GC9A01A driver

### Development Sequence
1. Configure core system (ESPHome, PSRAM)
2. Set up communication buses (SPI, I2C)
3. Implement display driver (custom GC9A01A)
4. Add touch functionality (CST816)
5. Enhance with advanced graphics (LVGL, Display Menu)