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
External momentary tactile buttons for starting and pausing the game.
| Button | ATmega328P Pin | Type | Description |
| :--- | :--- | :--- | :--- |
| **START Button** | PD3 | Digital Input | Start game / Select option (internal pull-up) |
| **PAUSE Button** | PB0 | Digital Input | Pause/unpause gameplay (internal pull-up) |

---

## Adding your files
Place your schematics and circuit diagrams here:
- **Formats**: PNG, PDF, or Fritzing `.fzz` files.
- **Reference**: Once uploaded, link them directly in the main [README.md](../README.md).