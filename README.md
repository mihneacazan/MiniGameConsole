# MiniGameConsole - Microprocessor Design (PM) Project

A portable, retro game console built around the **ATmega328P** microcontroller (using the ATmega328P Xplained Mini development board). The project implements classic games like **Flappy Bird** and **Breakout**, controlled via an analog joystick, offering visual feedback via an OLED display and an RGB LED, and audio feedback using a passive buzzer.

---

## 🎮 General Features & Gameplay

The console provides a fully interactive gaming experience:
* **Interactive Main Menu**: Allows navigating and selecting the desired game (Breakout or Flappy Bird), setting the player's name, and viewing the high scores leaderboard.
* **EEPROM High-Score Saving**: Maximum scores are stored permanently in the microcontroller's non-volatile EEPROM, persisting even after the console is powered off.
* **Flappy Bird**:
  - Adaptive difficulty (scroll speed increases as your score goes up, and the gap between pipes shrinks).
  - Sound effects for wing flaps (jumps) and scoring.
  - Optimized framebuffer graphics (eliminating all lag).
* **Breakout**:
  - Smooth paddle control using the analog joystick.
  - Dynamic brick destruction with a lives system and adaptive ball speed.
* **Advanced Feedback System**:
  - **Visual (RGB LED)**: Indicates game state (Green for start/low score, Orange/Yellow for medium score, Red for high score/Game Over, White flash on point collection).
  - **Audio (Buzzer)**: Action-specific sounds (jump, ball collision, brick destruction, descending game-over melody).
* **Pause Menu**: Games can be paused at any time using the joystick click, while the **BACK** and **HOME** tactile buttons allow for menu navigation and quick reset back to the home screen respectively.

---

## 🛠️ Hardware Components Used

1. **Microcontroller**: ATmega328P Xplained Mini (16 MHz clock frequency).
2. **SSD1306 OLED Display**: 128x64 pixel resolution, communicating via the **I2C** protocol.
3. **Dual-Axis Analog Joystick (KY-023)**: Used for menu navigation, paddle control in Breakout, and jumping in Flappy Bird.
4. **RGB LED (Common Cathode)**: For real-time status feedback and visual notifications.
5. **Passive Buzzer**: For generating tunes and tones (using Timer 1 in CTC mode).
6. **External Tactile Buttons**:
   - **BACK Button** (Pin PD3)
   - **HOME Button** (Pin PB0)
7. **Other Accessories**: 220Ω current-limiting resistors for the RGB LED, breadboard, Dupont connection wires, and a custom designed enclosure.

---

## 🔌 Connections & Electrical Schematic

All details regarding pin mapping and electrical connections between the components and the ATmega328P board can be found in the dedicated guide:
👉 **[Hardware Wiring & Pins Guide](hardware/README.md)**

### Block Diagram
![Block Diagram](hardware/image-2.png)

### Electrical Schematic
![Electrical Schematic](hardware/image-3.png)

---

## 📂 Repository Structure

```bash
├── src/               # C source code files (.c and .h)
│   ├── main.c         # Main menu, main game loop, and state machine
│   ├── flappy.c/h     # Flappy Bird game logic and rendering
│   ├── breakout.c/h   # Breakout game logic and rendering
│   ├── ssd1306.c/h    # SSD1306 OLED driver (optimized for fast rendering)
│   ├── adc.c/h        # Reading analog values from the joystick
│   ├── buzzer.c/h     # Tone generation using Timer 1 in CTC mode
│   ├── pwm.c/h        # RGB LED control using Timer 0 and Timer 2 (PWM)
│   ├── i2c.c/h        # Hardware I2C (TWI) protocol implementation
│   └── eeprom_scores.c/h # Saving and loading high scores to/from EEPROM
├── hardware/          # Electrical schematics, wiring schematics, and pin documentation
├── images/            # Photos of the physical console and enclosure
└── platformio.ini     # PlatformIO configuration file for ATmega328P
```

---

## 💻 Software Implementation Details

The application is written entirely in **C (AVR-GCC)** without relying on the heavy Arduino framework, writing directly to microcontroller registers for maximum performance.

### Key Optimizations:
* **Screen Optimization (Buffer Fill)**: The rectangle filling function `ssd1306_fill_rect` has been optimized to operate directly at the byte level in the buffer memory, reducing instructions from ~3000 pixel drawing calls per frame to just ~200. This completely eliminates lag in Flappy Bird at higher speeds.
* **Non-blocking Audio Control**: The buzzer uses hardware compare interrupts (Timer 1, toggling the OC1A pin) allowing tones to play in the background without blocking the processor with delay functions.
* **Pseudo-Random Number Generator (PRNG)**: The seed for generating pipes is obtained from analog noise read from unconnected ADC pins.

---

## 🚀 Building & Running

The project is built using **PlatformIO**. To upload it to the board:

1. Install the **PlatformIO IDE** extension in VS Code.
2. Connect the **ATmega328P Xplained Mini** board to your computer using a Micro-USB cable.
3. Open the project folder in VS Code.
4. Click on the PlatformIO icon on the left sidebar and select **Upload** (or run `platformio run --target upload` in the terminal).

### Running Individual Hardware Tests
In the [main.c](src/main.c) file, there is a macro named `TEST_TASK`. You can change its value to test separate modules of the console:
* `1` - RGB LED Test (color cycle)
* `2` - Joystick Test (X/Y values sent via USART + LED color changes on click)
* `3` - Buzzer Test (plays a musical scale)
* `4` - OLED Screen Test (draws geometric shapes)
* `5` - Buttons Test (BACK/HOME with USART output)
* `6` - **Full Game Console** (Default mode)

---

## 📸 Project Photos
Photos of the physical design and the final case can be viewed directly in:
👉 **[Project Photo Gallery](images/README.md)**

---
*Project built for the Microprocessor Design course, Faculty of Automatic Control and Computers, National University of Science and Technology Politehnica Bucharest.*
