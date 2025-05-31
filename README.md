# ECE362 Project Brick Breaker
Project Overview: This project implements a classic Brick Breaker (or “Breakout”) game on an STM32F091 microcontroller. A TFT LCD is used to display the game, and an APDS-9960 motion sensor controls the paddle via gesture recognition. When the game ends, high scores are stored in an external EEPROM over I2C, ensuring that data is retained even when the system is powered off. Sound effects (e.g., collisions, brick breaks) are generated using PWM and/or the DAC.

Game Objective:
1. Use the on-screen paddle to bounce a ball and break bricks.
2. Score points for each brick you break, and try to beat the stored high score.

Key Features:
1. Gesture - Based Paddle Movement - The APDS-9960 motion sensor detects hand movements (left, right, up, down) through I2C, and moves the paddle accordingly on the TFT LCD.
2. Dynamic Game Display - A TFT LCD screen (connected over SPI) shows the bricks, paddle, ball, score, and game status.
3. Sound Effects - Simple collision and brick-breaking sounds are generated using PWM (or DAC) to drive a buzzer/speaker.
4. High Score Persistence - An external EEPROM (I2C-based) saves and retrieves high scores, so they remain even after power is turned off.

Hardware Requirements:
1. Microcontroller- STM32F091 
2. Display - TFT LCD with SPI interface
3. Gesture Sensor - APDS-9960 
4. EEPROM - 24CXX series or similar I2C EEPROM (address typically 0x50 or nearby, depending on A0–A2 pins)
5. Speaker/Buzzer - Driven by PWM or DAC output on the STM32
