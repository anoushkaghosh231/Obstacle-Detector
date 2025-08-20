# Obstacle-Detector
This is an Arduino based hardware project for obstacle detection. It includes lock mechanism and warning with LED indication. The simulated project in TinkerCad is attached for information about port connections, components and sample output. Head to README for more info.

Simulation:
```[https://www.tinkercad.com/things/iNdjR9U1FCI-obstacle-detector?sharecode=YJH1S6S0Bw-kkI10vneZm6yNKUPkL78qRGHoVy_ycVM](https://www.tinkercad.com/things/gJTYEuBdfa9-copy-of-obstacle-detector?sharecode=5xX88A_rMy8DRdn9gv8SWU748ckgTFzCSn9Z7HJcVk8)](https://www.tinkercad.com/things/gJTYEuBdfa9-copy-of-obstacle-detector?sharecode=undefined)```

<img width="1463" height="746" alt="image" src="https://github.com/user-attachments/assets/c2ab49a3-cc6e-4a40-88b4-a92b0fecb077" />

## Description
<br>
This ultrasonic distance measurement system is a feature-rich embedded solution designed for real-time proximity sensing, user interaction, and safety control. Overall, the system operates in three modes: normal, locked, and settings, each with distinct behaviors, making it a robust and user-friendly tool for distance monitoring and safety alerts.

## Functionalities
<br>
- It uses an ultrasonic sensor to measure distances up to 400 cm with interrupt-driven echo timing for precision. <br>
- The readings are smoothed using a weighted filter and displayed on a 16x2 LCD, which also provides status updates and warnings. <br>
- A push button enables user control, allowing unlocking of the system when it auto-locks due to nearby objects (within 50 cm), and toggling between distance and settings modes. <br>
- The system includes dual LEDs—one for warnings that blink faster as objects get closer, and another for error indication during locked states. <br>
- The interface is responsive and intuitive, with debounced button input and visual feedback. <br>
- Non-blocking timing via millis() ensures smooth performance, and EEPROM recovery on startup maintains user settings. 

