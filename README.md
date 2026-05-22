# MiniGameConsole - Proiect Proiectare cu Microprocesoare (PM)

O consolă de jocuri portabilă, retro, construită în jurul microcontrolerului **ATmega328P** (placa de dezvoltare ATmega328P Xplained Mini). Proiectul implementează jocuri clasice precum **Flappy Bird** și **Breakout**, controlate printr-un joystick analogic, oferind feedback vizual prin intermediul unui ecran OLED și a unui LED RGB, precum și feedback auditiv folosind un buzzer pasiv.

---

## 🎮 Prezentare Generală și Funcționalități

Consola oferă o experiență interactivă completă de gaming:
* **Meniu Principal Interactiv**: Permite navigarea și selecția jocului dorit (Breakout sau Flappy Bird), setarea numelui jucătorului și vizualizarea clasamentului.
* **Salvare High-Scores în EEPROM**: Scorurile maxime obținute sunt stocate permanent în memoria EEPROM a microcontrolerului, persistând chiar și după oprirea consolei.
* **Jocul Flappy Bird**:
  - Dificultate adaptivă (viteza de deplasare crește pe măsură ce scorul se mărește, iar spațiul liber dintre țevi se micșorează).
  - Efecte sonore la sărituri (flap) și scor.
  - Optimizare grafică direct pe frame-buffer (elimină orice formă de lag).
* **Jocul Breakout**:
  - Control fin al paletei prin intermediul joystick-ului analogic.
  - Distrugerea dinamică a cărămizilor cu un sistem de vieți și viteză adaptivă a bilei.
* **Sistem de Feedback Avansat**:
  - **Vizual (LED RGB)**: Indică starea jocului (Verde pentru început/scor mic, Portocaliu/Galben pentru scor mediu, Roșu pentru scor mare/Game Over, Flash alb la colectarea punctelor).
  - **Auditiv (Buzzer)**: Sunete specifice pentru acțiuni (săritură, lovirea bilei, distrugerea cărămizilor, melodie descrescătoare la Game Over).
* **Meniu de Pauză**: Jocurile pot fi întrerupte oricând utilizând butonul dedicat de PAUSE, oferind opțiuni de reluare, restart sau întoarcere la meniul principal.

---

## 🛠️ Componente Hardware Utilizate

1. **Microcontroler**: ATmega328P Xplained Mini (frecvență de ceas 16 MHz).
2. **Ecran OLED SSD1306**: Rezoluție 128x64 pixeli, comunicare prin protocolul **I2C**.
3. **Joystick Analogic XY (KY-023)**: Utilizat pentru navigare în meniu, controlul paletei în Breakout și săritura în Flappy Bird.
4. **LED RGB (Catod Comun)**: Pentru feedback de stare și notificări vizuale în timp real.
5. **Buzzer Pasiv**: Pentru generarea de melodii și tonuri (folosind Timer 1 în mod CTC).
6. **Butoane tactile externe**:
   - **START Button** (Pin PD3)
   - **PAUSE Button** (Pin PB0)
7. **Altele**: Rezistențe de 220Ω pentru protecția LED-ului RGB, breadboard, cabluri de legătură (dupont), carcasă proiectată personalizat.

---

## 🔌 Conexiuni și Schemă Electrică

Toate detaliile privind maparea pinilor și conexiunile electrice dintre componente și placa ATmega328P pot fi consultate în directorul dedicat:
👉 **[Hardware Wiring & Pins Guide](file:///c:/Users/mihne/OneDrive/Desktop/Facultate/Anul%203/PM/MiniGameConsole/hardware/README.md)**

*Schema electrică completă (PDF/Fritzing) și poze din timpul asamblării se găsesc în folderul [hardware/](file:///c:/Users/mihne/OneDrive/Desktop/Facultate/Anul%203/PM/MiniGameConsole/hardware).*

---

## 📂 Structura Depozitului (Repository)

```bash
├── src/               # Codul sursă C (.c și .h) al proiectului
│   ├── main.c         # Meniul principal, bucla principală și controlul stărilor
│   ├── flappy.c/h     # Logica și randarea jocului Flappy Bird
│   ├── breakout.c/h   # Logica și randarea jocului Breakout
│   ├── ssd1306.c/h    # Driver OLED SSD1306 (optimizat pentru randare rapidă)
│   ├── adc.c/h        # Citirea valorilor analogice ale joystick-ului
│   ├── buzzer.c/h     # Generarea de tonuri folosind Timer1 în mod CTC
│   ├── pwm.c/h        # Controlul LED-ului RGB folosind Timer0 și Timer2 (PWM)
│   ├── i2c.c/h        # Implementarea protocolului I2C hardware (TWI)
│   └── eeprom_scores.c/h # Salvarea și încărcarea scorurilor în EEPROM
├── hardware/          # Scheme electrice, scheme de conexiuni și documentație pini
├── images/            # Fotografii cu consola fizică și carcasa acesteia
└── platformio.ini     # Fișierul de configurare PlatformIO pentru ATmega328P
```

---

## 💻 Detalii de Implementare Software

Aplicația este scrisă integral în **C (AVR-GCC)** fără a depinde de framework-ul greoi Arduino, scriind direct în regiștrii microcontrolerului pentru performanță maximă.

### Optimizări Cheie:
* **Optimizare Ecran (Buffer Fill)**: Funcția de umplere a dreptunghiurilor `ssd1306_fill_rect` a fost complet optimizată pentru a opera direct pe octeți (byte level) în memoria buffer, reducând numărul de instrucțiuni de la ~3000 de apeluri de pixeli per cadru la doar ~200. Aceasta elimină complet lag-ul din Flappy Bird la viteze ridicate.
* **Control Audio fără Bloca**: Buzzer-ul utilizează întreruperi de comparare hardware (Timer 1, toggling pin OC1A) permițând redarea tonurilor în fundal fără a bloca procesorul cu funcții de tip `delay`.
* **Generator de Numere Pseudo-Aleatoare (PRNG)**: Seed-ul pentru generarea țevilor este preluat din zgomotul analogic citit de pe pinii ADC neconectați.

---

## 🚀 Compilare și Rulare

Proiectul folosește **PlatformIO**. Pentru a-l încărca pe placă:

1. Instalează extensia **PlatformIO** în VS Code.
2. Conectează placa **ATmega328P Xplained Mini** la calculator prin cablul Micro-USB.
3. Deschide proiectul în VS Code.
4. Apasă pe pictograma PlatformIO din stânga și selectează **Upload** (sau rulează `platformio run --target upload` în terminal).

### Rularea Testelor Hardware Individuale
În fișierul [main.c](file:///c:/Users/mihne/OneDrive/Desktop/Facultate/Anul%203/PM/MiniGameConsole/src/main.c#L21) există o macrou numit `TEST_TASK`. Poți schimba valoarea acestuia pentru a testa module separate ale consolei:
* `1` - Test LED RGB (ciclu de culori)
* `2` - Test Joystick (valori X/Y trimise prin USART + schimbare culori LED la click)
* `3` - Test Buzzer (redă o gamă muzicală)
* `4` - Test Ecran OLED (desenează forme geometrice)
* `5` - Test Butoane (START/PAUSE cu afișare USART)
* `6` - **Consola de Jocuri Completă** (Mod implicit)

---

## 📸 Fotografii Proiect
Fotografiile cu designul fizic și carcasa finală pot fi vizualizate direct în:
👉 **[Galeria Foto Proiect](file:///c:/Users/mihne/OneDrive/Desktop/Facultate/Anul%203/PM/MiniGameConsole/images/README.md)**

---
*Proiect realizat pentru disciplina Proiectare cu Microprocesoare, Facultatea de Automatică și Calculatoare, Universitatea Națională de Știință și Tehnologie Politehnica București.*
