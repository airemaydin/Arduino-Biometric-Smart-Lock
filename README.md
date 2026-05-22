# Arduino-Based Biometric Smart Lock System

## Project Overview
This project is an Arduino-based smart lock system that utilizes fingerprint verification for secure access control. It integrates hardware components (biometric sensor, servo motor, I2C LCD) with a software architecture featuring dynamic memory allocation for user IDs and software debounce algorithms for physical hardware stability.

## Key Technical Features
* **Dynamic ID Allocation:** The system automatically scans the sensor's memory and assigns the first available empty ID slot to new users, preventing overwrite errors that occur in basic template-counting methods.
* **Interactive Deletion Menu:** A hybrid UI allowing administrators to delete users either by scanning the registered finger or by navigating through currently active IDs using hardware buttons.
* **Software Debounce (Signal Stability):** Implemented debounce logic (`delay` and `while` loop locks) for physical buttons to prevent multiple triggers and ensure stable state transitions.
* **Serial Port Admin Override:** Enables administrators to bypass physical inputs and delete specific user IDs directly via the Serial Monitor (`Serial.parseInt()`).
* **Visual & Audio Feedback:** Integrated a 16x2 I2C LCD, Red/Green LEDs, and a buzzer for real-time system status, access denial, and error handling.

## Hardware Components
* Arduino Uno
* Adafruit Optical Fingerprint Sensor
* 16x2 I2C LCD Display
* Servo Motor (Door lock simulation mechanism)
* Push Buttons, LEDs, Buzzer, Breadboard & Jumper Wires

## Circuit Diagram & Physical Prototype

![Circuit Diagram](circuit_diagram.png)

![Physical Prototype](prototype.png)

## Technical Challenges Solved
During the development and testing phase, several hardware-software integration challenges were successfully resolved:
1. **Button Bouncing:** Prevented infinite loops and accidental cancellations by writing strict software debounce filters.
2. **Memory Collision:** Solved the issue of new fingerprints overwriting old ones after a deletion by building an algorithm that targets exact empty memory addresses.
3. **Serial Port Conflicts:** Migrated the fingerprint sensor to digital pins (D2, D3) using `SoftwareSerial` to keep the hardware RX/TX pins free for debugging and PC communication.
