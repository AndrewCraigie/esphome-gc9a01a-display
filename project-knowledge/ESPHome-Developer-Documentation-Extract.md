# ESPHome Developer Documentation - Complete Reference

**Collated from:** ESPHome Developer Documentation  
**Source URLs:** 
- https://developers.esphome.io/architecture/api/
- https://developers.esphome.io/architecture/api/protocol_details/
- https://developers.esphome.io/architecture/components/
- https://developers.esphome.io/architecture/components/gpio/
- https://developers.esphome.io/architecture/components/i2c/
- https://developers.esphome.io/architecture/components/spi/
- https://developers.esphome.io/architecture/components/uart/

---

## Table of Contents

1. [API Architecture](#api-architecture)
2. [API Protocol Details](#api-protocol-details)
3. [Component Architecture](#component-architecture)
4. [Hardware Interface Components](#hardware-interface-components)
   - [GPIO Interface](#gpio-interface)
   - [I2C Interface](#i2c-interface)
   - [SPI Interface](#spi-interface)
   - [UART/Serial Interface](#uartserial-interface)

---

# API Architecture

ESPHome uses [Protocol Buffers (commonly known as "Protobuf")](https://protobuf.dev). This mechanism facilitates serializing messages sent between the application running on the device and [Home Assistant](http://home-assistant.io).

## Modifying the API

When making changes to the API, it's important that both sides (ESPHome and Home Assistant) are updated together. At a high level, changes must be made to:

- [ESPHome](https://github.com/esphome/esphome)
- [aioesphomeapi](https://github.com/esphome/aioesphomeapi)
- [Home Assistant core](https://github.com/home-assistant/core)
- [Home Assistant docs](https://github.com/home-assistant/home-assistant.io), if adding/changing a user-facing platform/feature

## ESPHome API Structure

API component directory structure:

```
esphome
├─ components
│ ├─ api
│ │ ├─ api_connection.cpp
│ │ ├─ api_connection.h
│ │ ├─ api_frame_helper.cpp
│ │ ├─ api_frame_helper.h
│ │ ├─ api_noise_context.h
│ │ ├─ api_options.proto
│ │ ├─ api_pb2_service.cpp
│ │ ├─ api_pb2_service.h
│ │ ├─ api_pb2.cpp
│ │ ├─ api_pb2.h
│ │ ├─ api_server.cpp
│ │ ├─ api_server.h
│ │ ├─ api.proto
│ │ ├─ client.py
│ │ ├─ custom_api_device.h
│ │ ├─ homeassistant_service.h
│ │ ├─ list_entities.cpp
│ │ ├─ list_entities.h
│ │ ├─ proto.cpp
│ │ ├─ proto.h
│ │ ├─ subscribe_state.cpp
│ │ ├─ subscribe_state.h
│ │ ├─ user_services.cpp
│ │ ├─ user_services.h
```

The `.proto` files contain the protobuf definitions; these files should be modified as necessary to accommodate changes to the API.

After editing the `.proto` files, the `script/api_protobuf/api_protobuf.py` script should be run. This script will regenerate the `api_pb2` files based on the changes made in the `.proto` files.

**Do not edit the `api_pb2` files manually.**

## Related Components

### aioesphomeapi
This is the Python client for ESPHome which runs in Home Assistant; it must be updated to be consistent with the changes made to ESPHome itself.

### Home Assistant core
Support must be implemented to integrate the ESPHome implementation into Home Assistant.

### Home Assistant docs
If user-facing functionality has been added and/or changed, the documentation must be updated so that users can understand the functionality.

---

# API Protocol Details

This document describes the low-level protocol formats used by the ESPHome API, including message framing, buffer layouts, and wire formats for both Noise and Plaintext protocols.

## Overview

The ESPHome API supports two protocol variants:

- **Noise Protocol**: Encrypted communication using the Noise protocol framework
- **Plaintext Protocol**: Unencrypted communication with variable-length encoding

Both protocols use protocol buffers for message serialization but differ in their framing and transport layer.

## Common Elements

### Message Types
- Message types are encoded as 16-bit unsigned values (two bytes)
- Split into `type_high` (upper byte) and `type_low` (lower byte)
- Allows for 65,536 different message types (0-65535)

### VarInt Encoding
Variable-length integer encoding follows the [Protocol Buffers VarInt specification](https://protobuf.dev/programming-guides/encoding/):

- Each byte has a continuation bit (MSB)
- 7 bits of data per byte
- Least significant bits first
- Always encodes unsigned values

**Size ranges:**
- 1 byte: 0-127
- 2 bytes: 128-16,383
- 3 bytes: 16,384-2,097,151
- 4 bytes: 2,097,152-268,435,455
- 5 bytes: 268,435,456-34,359,738,367

## Noise Protocol

### Overview
The Noise protocol provides encrypted, authenticated communication using the [Noise XX handshake pattern](https://www.noiseprotocol.org/noise.html#interactive-handshake-patterns-fundamental).

### Frame Structure
```
[Indicator][Encrypted Size][Encrypted Payload][MAC]
   1 byte      2 bytes        Variable       16 bytes
```

### Message Format

**Unencrypted Header (3 bytes)**
- Indicator: 0x01
- Encrypted payload size: 16-bit unsigned, big-endian

**Encrypted Payload**
- Message type: 16-bit unsigned, big-endian (encrypted)
- Data length: 16-bit unsigned, big-endian (encrypted)
- Protocol buffer data

**MAC (16 bytes)**

### Data Type Summary

| Field | Type | Size | Encoding | Notes |
|-------|------|------|----------|-------|
| Indicator | uint8 | 1 byte | - | Always 0x01 |
| Encrypted Size | uint16 | 2 bytes | Big-endian | Not encrypted |
| Message Type | uint16 | 2 bytes | Big-endian | Encrypted |
| Data Length | uint16 | 2 bytes | Big-endian | Encrypted |
| Data | bytes | Variable | - | Protocol buffer payload, encrypted |

### Buffer Layout
```
Position: [0] [1] [2] [3] [4] [5] [6] [7] ... [N-15] ... [N]
Content:  Ind ES₁ ES₂ MT₁ MT₂ DL₁ DL₂ [Payload Data] [MAC-16-bytes]
          └─────── Header (7 bytes) ──────┘ └─ Footer ─┘
```

Where:
- `Ind`: Indicator byte (0x01 for Noise)
- `ES₁/ES₂`: Encrypted payload size (16-bit unsigned, big-endian)
- `MT₁/MT₂`: Message type (16-bit unsigned, big-endian, encrypted)
- `DL₁/DL₂`: Data length (16-bit unsigned, big-endian, encrypted)
- `Payload`: Actual message data (encrypted)
- `MAC`: 16-byte authentication tag

### Encryption Details
- Uses ChaCha20-Poly1305 AEAD cipher
- Encrypts: message type + data length + payload
- The encrypted size field in the header is NOT encrypted
- MAC provides authentication for the entire encrypted payload

### State Machine

The Noise protocol uses the following states:

- **INITIALIZE**: Initial state, waiting for init() to be called
- **CLIENT_HELLO**: Waiting for client hello message
- **SERVER_HELLO**: Sending server hello with device info
- **HANDSHAKE**: Performing Noise XX handshake exchange
- **DATA**: Handshake complete, ready for encrypted data exchange
- **CLOSED**: Connection closed normally
- **FAILED**: Error occurred, connection failed
- **EXPLICIT_REJECT**: Temporary state for sending handshake rejection

### Handshake Process

During the Noise handshake, the server sends a SERVER_HELLO message:

**SERVER_HELLO format:**
```
[Indicator] [Size] [Protocol] [Node-Name] [MAC-Address]
    0x01     2B      0x01     null-term    null-term
```

**Example SERVER_HELLO:**
```
Hex: 01 00 1C 01 65 73 70 68 6F 6D 65 00 31 32 3A 33 34 3A 35 36 3A 37 38 3A 39 41 3A 42 43 00
     ^  ^^^^^  ^  ^--------node------^  ^  ^-----------------MAC address--------------------^  ^
     |    |    |                     |  |                                                     |
     |    |    Protocol (0x01)    null null                                                  null
     |    Size (28 bytes, big-endian)
     Indicator (0x01)
```

This decodes to:
- Frame indicator: 0x01
- Frame size: 0x001C (28 bytes)
- Protocol: 0x01 (always)
- Node name: "esphome" (null-terminated)
- MAC: "12:34:56:78:9A:BC" (null-terminated)

### Error Handling

**Handshake rejection format:**
```
[Indicator] [Size] [Error-Flag] [Error-Message]
    0x01     2B       0x01        Variable
```

**Example handshake rejection:**
```
Hex: 01 00 17 01 48 61 6E 64 73 68 61 6B 65 20 4D 41 43 20 66 61 69 6C 75 72 65
     ^  ^^^^^  ^  ^----------------------Error message------------------------^
     |    |    |
     |    |    Error flag (0x01)
     |    Size (23 bytes, big-endian)
     Indicator (0x01)
```

This decodes to:
- Frame indicator: 0x01
- Frame size: 0x0017 (23 bytes)
- Error flag: 0x01 (failure)
- Error message: "Handshake MAC failure" (NOT null-terminated)

### Wire Format Example

Sending a temperature reading (value: 23.5°C):
```
Hex: 01 00 0E 00 08 00 06 12 04 08 96 42 10 B4 46
     [C H A C H A 2 0 - P O L Y M A C - 1 6 bytes]
```

- `01`: Noise indicator
- `00 0E`: Encrypted size (14 bytes, big-endian unsigned)
- Encrypted payload (before encryption):
  - `00 08`: Message type 8 (big-endian unsigned)
  - `00 06`: Data length 6 (big-endian unsigned)
  - `12 04 08 96 42 10`: Protocol buffer data
- 16-byte MAC appended

## Plaintext Protocol

### Overview
The plaintext protocol uses variable-length encoding to minimize overhead for unencrypted communication.

### State Machine

The plaintext protocol uses a simpler state machine:

- **INITIALIZE**: Initial state, waiting for init() to be called
- **DATA**: Ready for data exchange (no handshake required)
- **CLOSED**: Connection closed normally
- **FAILED**: Error occurred, connection failed

No handshake is required for the plaintext protocol - it transitions directly to the DATA state after initialization.

### Frame Structure
```
[Indicator][Payload Size VarInt][Message Type VarInt][Payload]
   1 byte        1-3 bytes           1-2 bytes        Variable
```

### Data Type Summary

| Field | Type | Size | Encoding | Notes |
|-------|------|------|----------|-------|
| Indicator | uint8 | 1 byte | - | Always 0x00 |
| Payload Size | varint | 1-3 bytes | [VarInt](https://protobuf.dev/programming-guides/encoding/) | Protocol buffer payload size |
| Message Type | varint | 1-2 bytes | VarInt | Message type identifier |
| Payload | bytes | Variable | - | Protocol buffer data |

### Buffer Layout

The plaintext protocol dynamically adjusts the header position to minimize padding:

**Small Messages (header length = 3 bytes)**
```
Position: [0] [1] [2] [3] [4] [5] [6] ...
Content:  XX  XX  XX  Ind FSz MTp [Payload]
          └ Padding ┘  └── Header ──┘
                       (offset=3)
```

**Medium Messages (header length = 4 bytes)**
```
Position: [0] [1] [2] [3] [4] [5] [6] ...
Content:  XX  XX  Ind FS₁ FS₂ MTp [Payload]
          └Pad┘   └─── Header ────┘
                  (offset=2)
```

**Large Messages (header length = 6 bytes)**
```
Position: [0] [1] [2] [3] [4] [5] [6] ...
Content:  Ind FS₁ FS₂ FS₃ MT₁ MT₂ [Payload]
          └────── Header ───────┘
          (offset=0, no padding)
```

Where:
- `Ind`: Indicator byte (0x00 for plaintext)
- `FSz/FS₁/FS₂/FS₃`: Frame size varint (unsigned)
- `MTp/MT₁/MT₂`: Message type varint (unsigned)
- Padding bytes are unused (can be any value)

### Wire Format Example

Same temperature reading:
```
Hex: 00 06 08 12 04 08 96 42 10
```

- `00`: Plaintext indicator
- `06`: Payload size (6 bytes, varint unsigned)
- `08`: Message type 8 (varint unsigned)
- `12 04 08 96 42 10`: Protocol buffer data

## Performance Considerations

### Noise Protocol
- Fixed 23-byte overhead (7 header + 16 MAC)
- Constant-time header construction
- Higher CPU usage for encryption
- Better for security-sensitive applications

### Plaintext Protocol
- Variable 3-5 byte overhead (depending on sizes)
- Dynamic header positioning
- Lower CPU usage
- Better for high-frequency, non-sensitive data

## Implementation Notes

### Integer Types
All size and type fields are unsigned integers

### Endianness
- Noise protocol: All multi-byte values use big-endian encoding
- Plaintext protocol: Uses [VarInt encoding](https://protobuf.dev/programming-guides/encoding/) (Protocol Buffers standard)

### Buffer Alignment
Both protocols ensure payload data starts at predictable offsets for efficient processing

### Error Handling
- Invalid frame indicators or sizes should immediately close the connection
- For Noise protocol specific errors (handshake failures, decryption errors, etc.), see the Noise Protocol section above

### Maximum Sizes
- Message types: 0-65,535 (16-bit unsigned)
- Frame/data sizes: up to 2^32-1 bytes (varint can encode up to 64-bit values, but practically limited by memory)

---

# Component Architecture

All components within ESPHome have a specific structure. This structure exists because:

- It allows the Python parts of ESPHome to:
  - Easily determine which parts of the C++ codebase are required to complete a build.
  - Understand how to interact with the component/platform so it can be configured correctly.
- It makes understanding and maintaining the codebase easier.

## Directory Structure

```
esphome
├─ components
│ ├─ example_component
│ │ ├─ __init__.py
│ │ ├─ example_component.h
│ │ ├─ example_component.cpp
```

This is the most basic component directory structure where the component would be used at the top-level of the YAML configuration.

```yaml
example_component:
```

## Python Module Structure

### Minimum Requirements

At the heart of every ESPHome component is the `CONFIG_SCHEMA` and the `to_code` method.

The `CONFIG_SCHEMA` is based on and extends [Voluptuous](https://github.com/alecthomas/voluptuous), which is a data validation library. This allows the YAML to be parsed and converted to a Python object and performs strong validation against the data types to ensure they match.

```python
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

CONF_FOO = "foo"
CONF_BAR = "bar"
CONF_BAZ = "baz"

example_component_ns = cg.esphome_ns.namespace("example_component")
ExampleComponent = example_component_ns.class_("ExampleComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ExampleComponent),
    cv.Required(CONF_FOO): cv.boolean,
    cv.Optional(CONF_BAR): cv.string,
    cv.Optional(CONF_BAZ): cv.int_range(0, 255),
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    cg.add(var.set_foo(config[CONF_FOO]))
    if bar := config.get(CONF_BAR):
        cg.add(var.set_bar(bar))
    if baz := config.get(CONF_BAZ):
        cg.add(var.set_baz(baz))
```

### Module/Component Setup

```python
import esphome.config_validation as cv
import esphome.codegen as cg
```

`config_validation` is a module that contains all the common validation method that are used to validate the configuration. Components may contain their own validations as well and this is very extensible. `codegen` is a module that contains all the code generation method that are used to generate the C++ code that is placed into `main.cpp`.

```python
example_component_ns = cg.esphome_ns.namespace("example_component")
```

This is the C++ namespace inside the `esphome` namespace. It is required here so that the codegen knows the exact namespace of the class that is being created. The namespace must match the name of the component.

```python
ExampleComponent = example_component_ns.class_("ExampleComponent", cg.Component)
```

This is the class that is being created. The first argument is the name of the class and any subsequent arguments are the base classes that this class inherits from.

### Configuration Validation

```python
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ExampleComponent),
    cv.Required(CONF_FOO): cv.boolean,
    cv.Optional(CONF_BAR): cv.string,
    cv.Optional(CONF_BAZ): cv.int_range(0, 255),
})
```

This is the schema that will allow user configuration for this component. This example requires the user to provide a boolean value for the key `foo`. The user also may or may not (hence `Optional`) provide a string value for the key `bar` and an integer value between 0 and 255 for they key `baz`. The `cv.GenerateID()` is a special function that generates a unique ID (C++ variable name used in the generated code) for this component but also allows the user to specify their own `id` in their configuration in the event that they wish to refer to it in their automations.

### Code Generation

The `to_code` method is called after the entire configuration has been validated. It is given the parsed `config` object for this instance of this component and uses it to determine exactly what C++ code is placed into the generated `main.cpp` file. It translates the user configuration into the C++ instance method calls, setting variables on the object as required/specified.

```python
var = cg.new_Pvariable(config[CONF_ID])
```

`var` becomes a special object that represents the actual C++ object that will be generated. The `CONF_ID` that represents the above `cv.GenerateID()` contains both the `id` string and the class name of the component -- in our example, this is `ExampleComponent`. Subsequent arguments to `new_Pvariable` are arguments that can be passed to the constructor of the class.

```python
await cg.register_component(var, config)
```

This line generates `App.register_component(var)` in C++ which registers the component so that its `setup`, `loop` and/or `update` methods are called correctly.

Assuming the user has `foo: true` in their YAML configuration, this line:

```python
cg.add(var.set_foo(config[CONF_FOO]))
```

...will result in this line:

```cpp
var->set_foo(true);
```

...in the generated `main.cpp` file.

```python
if bar := config.get(CONF_BAR):
    cg.add(var.set_bar(bar))
```

If the user has set `bar` in the configuration, this line will generate the C++ code to call `set_bar` on the object. If the config value is not set, then we do not call the setter function.

## Further Information

- **AUTO_LOAD**: A list of components that will be automatically loaded if they are not already specified in the configuration. This can be a method that can be run with access to the `CORE` information like the target platform.
- **CONFLICTS_WITH**: A list of components which conflict with this component. If the user has one of them in their config, a validation error will be generated.
- **CODEOWNERS**: A list of GitHub usernames that are responsible for this component. `script/build_codeowners.py` will update the `CODEOWNERS` file.
- **DEPENDENCIES**: A list of components that this component depends on. If these components are not present in the configuration, validation will fail and the user will be shown an error.
- **MULTI_CONF**: If set to `True`, the user can use this component multiple times in their configuration. If set to a number, the user can use this component that number of times.
- **MULTI_CONF_NO_DEFAULT**: This is a special flag that allows the component to be auto-loaded without an instance of the configuration. An example of this is the `uart` component. This component can be auto-loaded so that all of the UART headers will be available but potentially there is no native UART instance, but one provided by another component such an an external i2c UART expander.

### Final Validation

ESPHome has a mechanism to run a final validation step after all of the configuration is initially deemed to be individually valid. This final validation gives an instance of a component the ability to check the configuration of any other components and potentially fail the validation stage if an important dependent configuration does not match.

For example, many components that rely on `uart` can use the `FINAL_VALIDATE_SCHEMA` to ensure that the `tx_pin` and/or `rx_pin` are configured.

## C++ Component Structure

Given the example Python code above, let's consider the following C++ code:

### example_component.h:

```cpp
#pragma once

#include "esphome/core/component.h"

namespace esphome {
namespace example_component {

class ExampleComponent : public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  
  void set_foo(bool foo) { this->foo_ = foo;}
  void set_bar(std::string bar) { this->bar_ = bar;}
  void set_baz(int baz) { this->baz_ = baz;}

 protected:
  bool foo_{false};
  std::string bar_{};
  int baz_{0};
};

}  // namespace example_component
}  // namespace esphome
```

### example_component.cpp:

```cpp
#include "esphome/core/log.h"
#include "example_component.h"

namespace esphome {
namespace example_component {

static const char *TAG = "example_component.component";

void ExampleComponent::setup() {
  // Code here should perform all component initialization,
  // whether hardware, memory, or otherwise
}

void ExampleComponent::loop() {
  // Tasks here will be performed at every call of the main application loop.
  // Note: code here MUST NOT BLOCK (see below)
}

void ExampleComponent::dump_config(){
  ESP_LOGCONFIG(TAG, "Example component");
  ESP_LOGCONFIG(TAG, "  foo = %s", TRUEFALSE(this->foo_));
  ESP_LOGCONFIG(TAG, "  bar = %s", this->bar_.c_str());
  ESP_LOGCONFIG(TAG, "  baz = %i", this->baz_);
}

}  // namespace example_component
}  // namespace esphome
```

This represents the minimum required code to implement a component in ESPHome. While most of it is likely reasonably self-explanatory, let's walk through it for completeness.

### Namespaces

All components must have their own namespace, named appropriately based on the name of the component. The component's namespace will always be placed within the `esphome` namespace.

### Component Class

Any component exists as at least one C++ `class`. In ESPHome, components always inherit from either the `Component` or `PollingComponent` classes. The latter of these defines an additional `update()` method which is called on a periodic basis based on user configuration. This is often useful for hardware such as sensors which are queried periodically for a new measurement/reading.

### Common Methods

There are four methods `Component` defines which all components typically implement. They are as follows:

- **setup()**: This method is called once as ESPHome starts up to perform initialization of the component. This may mean simply initializing some memory/variables or performing a series of read/write calls to look for and initialize some (sensor, display, etc.) hardware connected via some bus (I2C, SPI, serial/UART, one-wire, etc.).

- **loop()**: This method is called at each iteration of ESPHome's main application loop. Typically this is every 16 milliseconds, but there may be some variance as other components consume cycles to perform their own tasks.

- **dump_config()**: This method is called as-needed to "dump" the device's current configuration. Typically this happens once after booting and then each time a new client connects to monitor logs (assuming logging is enabled). Note that this method is to be used only to dump configuration values determined during `setup()`; this method is not permitted to contain any other types of calls to (for example) perform bus reads and/or writes. We require that this method is implemented for all components.

- **get_setup_priority()**: This method is called to determine the component's setup priority. This is used specifically to ensure components are initialized in an appropriate order. For example, an I2C sensor cannot be initialized before the I2C bus is initialized; therefore, for I2C sensors, this must return a value indicating that it is to be initialized only after (I2C) busses are initialized. See `setup_priority` in `esphome/core/component.h` for commonly-used values.

In addition, for `PollingComponent`:

- **update()**: This method is called at an interval defined in the user's YAML configuration. For many components, the interval defaults to 60 seconds, but this may be overridden by the user to fit their use case.

**Warning:** Code in these methods (and elsewhere) must not block.

### Component-Specific Methods

"Setter" methods: it's common to have at least one configuration variable which must be defined by the user in order to configure a component/platform. In `ExampleComponent` (as above), we have three such variables: "foo", "bar" and "baz".

As mentioned earlier, these methods are the same methods referred from within the `to_code` function in Python; the values contained in the user's YAML configuration are passed through to these setter methods as they are placed into the generated `main.cpp` file produced by ESPHome's code generation (codegen). It's important to note that these methods will be called (and, thus, variables set) before the `setup()` method is called.

## Going Further

- To help you get started, we have a number of ["starter" components](https://github.com/esphome/starter-components).
- We encourage you to have a look at other [components](https://github.com/esphome/esphome/tree/dev/esphome/components) within ESPHome; it's often easiest to take something which already exists and modify it to fit your needs.
- If you're looking for information on a specific component type, see the navigation tree on the left.

---

# Hardware Interface Components

## GPIO Interface

We have an example, minimal component which uses a single GPIO pin [here](https://github.com/esphome/starter-components/tree/main/components/empty_gpio_component).

### Python Implementation

In addition to the usual requisite imports, we have:

```python
from esphome import pins
```

This allows us to use some functions we'll need for validation and code generation later on.

```python
empty_gpio_component_ns = cg.esphome_ns.namespace("empty_gpio_component")
EmptyGPIOComponent = empty_gpio_component_ns.class_("EmptyGPIOComponent", cg.Component)
```

This is some boilerplate code which allows ESPHome code generation to understand the namespace and class names that the component will use.

**CONFIG_SCHEMA**

This defines the configuration schema for the component as discussed in the Component Architecture section.

In particular, note that the schema includes `cv.Required(CONF_PIN): pins.gpio_output_pin_schema`. Of note:

- This specifically requires an output GPIO pin schema.
- While a complete pin schema is permitted, it is often shortened to `pin: GPIOXX`.
- This arrangement will allow any GPIO pin to be used -- including a GPIO pin which might be connected via an I/O expander! If you specifically require a pin that is "internal" to the microcontroller, you can use `internal_gpio_output_pin_schema`.
- If you require an input pin, simply swap `output` for `input` in `gpio_output_pin_schema`.

Finally, in the `to_code` function, we have:

```python
pin = await gpio_pin_expression(config[CONF_PIN])
cg.add(var.set_output_pin(pin))
```

This configures the component/platform to use the pin as specified in the YAML configuration.

### C++ Implementation

The C++ class for this example component is quite simple.

```cpp
class EmptyGPIOComponent : public Component { ... };
```

As mentioned in the contributing guidelines, all components/platforms must inherit from either `Component` or `PollingComponent`; our example here is no different.

The `void set_output_pin(GPIOPin *pin)` method is implemented to allow setting the pin that the component should use.

Finally, the component implements the usual set of methods as described in the Component Architecture section. This is all that's required for our minimal GPIO component!

## I2C Interface

We have an example, minimal I2C-based component [here](https://github.com/esphome/starter-components/tree/main/components/empty_i2c_component).

### Python Implementation

In addition to the usual requisite imports, we have:

```python
from esphome.components import i2c
```

This allows us to use some functions we'll need for validation and code generation later on.

```python
DEPENDENCIES = ["i2c"]
```

Given that this component will use--and consequently depend on--I2C, we must add it as a dependency. This allows ESPHome to understand this requirement and generate an error if it is not met.

```python
empty_i2c_component_ns = cg.esphome_ns.namespace("empty_i2c_component")
EmptyI2CComponent = empty_i2c_component_ns.class_(
    "EmptyI2CComponent", cg.Component, i2c.I2CDevice
)
```

This is some boilerplate code which allows ESPHome code generation to understand the namespace and class names that the component will use. Note that `EmptyI2CComponent` is inheriting from both `Component` and `I2CDevice`.

**CONFIG_SCHEMA**

This defines the configuration schema for the component as discussed in the Component Architecture section.

In particular, note that the schema is extended with `.extend(i2c.i2c_device_schema(0x01))` since this is an I2C component/platform. The `0x01` value is the default I2C address for the device to be used by this component/platform.

Finally, in the `to_code` function, we have:

```python
await i2c.register_i2c_device(var, config)
```

Since this is an I2C device, we must register it as such so it is handled appropriately by ESPHome.

### C++ Implementation

The C++ class for this example component is quite simple.

```cpp
class EmptyI2CComponent : public i2c::I2CDevice, public Component { ... };
```

As mentioned in the contributing guidelines, all components/platforms must inherit from either `Component` or `PollingComponent`; our example here is no different. Note that, since it's an I2C device, it also inherits from `I2CDevice`.

Finally, the component implements the usual set of methods as described in the Component Architecture section. This is all that's required for our minimal I2C component!

## SPI Interface

We have an example, minimal SPI-based component [here](https://github.com/esphome/starter-components/tree/main/components/empty_spi_component).

### Python Implementation

In addition to the usual requisite imports, we have:

```python
from esphome.components import spi
```

This allows us to use some functions we'll need for validation and code generation later on.

```python
DEPENDENCIES = ["spi"]
```

Given that this component will use--and consequently depend on--SPI, we must add it as a dependency. This allows ESPHome to understand this requirement and generate an error if it is not met.

```python
empty_spi_component_ns = cg.esphome_ns.namespace("empty_spi_component")
EmptySPIComponent = empty_spi_component_ns.class_(
    "EmptySPIComponent", cg.Component, spi.SPIDevice
)
```

This is some boilerplate code which allows ESPHome code generation to understand the namespace and class names that the component will use. Note that `EmptySPIComponent` is inheriting from both `Component` and `SPIDevice`.

**CONFIG_SCHEMA**

This defines the configuration schema for the component as discussed in the Component Architecture section.

In particular, note that the schema is extended with `.extend(spi.spi_device_schema(cs_pin_required=True))` since this is an SPI component/platform. Also note that we can configure whether or not a CS pin is required for this component.

Finally, in the `to_code` function, we have:

```python
await spi.register_spi_device(var, config)
```

Since this is an SPI device, we must register it as such so it is handled appropriately by ESPHome.

### C++ Implementation

The C++ class for this example component is quite simple.

```cpp
class EmptySPIComponent : public Component,
                         public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST,spi::CLOCK_POLARITY_LOW,
                                              spi::CLOCK_PHASE_LEADING,spi::DATA_RATE_1KHZ> { ... };
```

As mentioned in the contributing guidelines, all components/platforms must inherit from either `Component` or `PollingComponent`; our example here is no different. Note that, since it's an SPI device, it also inherits from `SPIDevice` which uses a C++ template to define the SPI bus parameters such as bit order, clock polarity and speed.

Finally, the component implements the usual set of methods as described in the Component Architecture section. This is all that's required for our minimal SPI component!

## UART/Serial Interface

We have an example, minimal UART-based component [here](https://github.com/esphome/starter-components/tree/main/components/empty_uart_component).

### Python Implementation

In addition to the usual requisite imports, we have:

```python
from esphome.components import uart
```

This allows us to use some functions we'll need for validation and code generation later on.

```python
DEPENDENCIES = ["uart"]
```

Given that this component will use--and consequently depend on--a UART, we must add it as a dependency. This allows ESPHome to understand this requirement and generate an error if it is not met.

```python
empty_uart_component_ns = cg.esphome_ns.namespace("empty_uart_component")
EmptyUARTComponent = empty_uart_component_ns.class_(
    "EmptyUARTComponent", cg.Component, uart.UARTDevice
)
```

This is some boilerplate code which allows ESPHome code generation to understand the namespace and class names that the component will use. Note that `EmptyUARTComponent` is inheriting from both `Component` and `UARTDevice`.

**CONFIG_SCHEMA**

This defines the configuration schema for the component as discussed in the Component Architecture section.

In particular, note that the schema is extended with `.extend(uart.UART_DEVICE_SCHEMA)` since this is a UART component/platform.

Finally, in the `to_code` function, we have:

```python
await uart.register_uart_device(var, config)
```

Since this is a serial device which uses a UART, we must register it as such so it is handled appropriately by ESPHome.

### C++ Implementation

The C++ class for this example component is quite simple.

```cpp
class EmptyUARTComponent : public uart::UARTDevice, public Component { ... };
```

As mentioned in the contributing guidelines, all components/platforms must inherit from either `Component` or `PollingComponent`; our example here is no different. Note that, since it's a UART device, it also inherits from `UARTDevice`.

Finally, the component implements the usual set of methods as described in the Component Architecture section. This is all that's required for our minimal UART component!

---

## Summary

This extract of the ESPHome Developer Documentation covers:

### Key Architecture Components:
- **API Layer**: Protocol Buffers communication with Home Assistant using both encrypted (Noise) and plaintext protocols
- **Component System**: Structured approach to building ESPHome components with Python configuration validation and C++ implementation
- **Hardware Interfaces**: Standardized patterns for GPIO, I2C, SPI, and UART communication

### Development Patterns:
- **Configuration Schemas**: Using Voluptuous for YAML validation
- **Code Generation**: Automatic C++ code generation from Python configuration
- **Component Lifecycle**: Setup, loop, update, and dump_config methods
- **Hardware Abstraction**: Device-specific classes for different communication protocols

### Protocol Details:
- **Message Framing**: Both variable-length (plaintext) and fixed-overhead (encrypted) formats
- **State Management**: Proper handshake and error handling procedures
- **Performance Optimization**: Dynamic header positioning and efficient buffer management

### Best Practices:
- **Dependency Management**: Proper component dependencies and auto-loading
- **Error Handling**: Comprehensive validation and graceful failure modes
- **Code Organization**: Namespace management and inheritance patterns
- **Hardware Integration**: Standardized interfaces for different bus types
