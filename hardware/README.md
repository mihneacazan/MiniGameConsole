# Hardware Schematics & Wiring Diagram

This directory contains the hardware-related assets for the **MiniGameConsole** project.

## Pin Mapping

The project is built on the **ATmega328P Xplained Mini** development board. Below is the complete pin connection table for all peripheral components.

### 1. I2C OLED Display (SSD1306)
The OLED display communicates via the hardware I2C bus.
| OLED Pin | ATmega328P Pin | Description |
| :--- | :--- | :--- |
| **GND** | GND | Ground |
| **VCC** | 5V / VCC | 5V Power Supply |
| **SCL** | PC5 (SCL) | I2C Clock Line |
| **SDA** | PC4 (SDA) | I2C Data Line |

### 2. Dual-Axis Joystick (KY-023)
The joystick uses analog inputs for positioning and a digital input for the click button.
| Joystick Pin | ATmega328P Pin | Channel / Type | Description |
| :--- | :--- | :--- | :--- |
| **GND** | GND | - | Ground |
| **+5V** | 5V / VCC | - | Power Supply |
| **VRX** | PC0 (ADC0) | Analog Input | X-Axis navigation (Left/Right) |
| **VRY** | PC1 (ADC1) | Analog Input | Y-Axis navigation (Up/Down) |
| **SW** | PD2 | Digital Input | Joystick button click (internal pull-up) |

### 3. RGB LED (Common Cathode)
The RGB LED uses PWM (Pulse-Width Modulation) to mix colors.
| RGB Pin | ATmega328P Pin | Timer / PWM Channel | Description |
| :--- | :--- | :--- | :--- |
| **R** (Red) | PD6 | OC0A (Timer 0) | Red Color Channel |
| **G** (Green) | PD5 | OC0B (Timer 0) | Green Color Channel |
| **B** (Blue) | PB3 | OC2A (Timer 2) | Blue Color Channel |
| **GND** | GND (via 220Ω resistor) | - | Ground |

### 4. Passive Buzzer
The buzzer uses CTC mode on Timer 1 to produce square wave sound effects at variable frequencies.
| Buzzer Pin | ATmega328P Pin | Timer Channel | Description |
| :--- | :--- | :--- | :--- |
| **Positive (+)** | PB1 | OC1A (Timer 1) | PWM Signal Output |
| **Negative (-)** | GND | - | Ground |

### 5. Control Buttons
External momentary tactile buttons for navigation and resetting the console.
| Button | ATmega328P Pin | Type | Description |
| :--- | :--- | :--- | :--- |
| **BACK Button** | PD3 | Digital Input | Navigate back / Return to previous menu OR Pause/unpause gameplay (internal pull-up) |
| **HOME Button** | PB0 | Digital Input | Reset console state / Return directly to game selection screen (internal pull-up) |

---

## 📊 Schematics & Diagrams

### 1. Block Diagram
The block diagram illustrates the basic architecture of the console and how the peripherals communicate with the ATmega328P microcontroller via I2C, ADC, PWM, and digital input/output interfaces.

![Block Diagram](image-2.png)

### 2. Electrical Schematic
The detailed electrical schematic presents the pin-to-pin connections between the ATmega328P Xplained Mini development board and all peripherals (OLED display, joystick, RGB LED, passive buzzer, and tactile buttons), along with the necessary current-limiting resistors.

![Electrical Schematic](image-3.png)