## Fuel Theft Detection
This code implements a fuel theft detection system using an ESP8266 microcontroller. It interfaces with an HC-SR04 ultrasonic sensor to measure the current fuel level in a tank and a vibration sensor to detect unusual activity. The system calculates the fuel level and displays it on an OLED screen. It continuously monitors the fuel level and triggers an interrupt service routine (ISR) if a significant drop in fuel is detected, indicating potential theft. The OLED display shows the current fuel level in centimeters.
![Prototype](./images/FTD.jpeg)

### Fuel level measurement
![Prototype](./images/Fuellevel.jpeg)
