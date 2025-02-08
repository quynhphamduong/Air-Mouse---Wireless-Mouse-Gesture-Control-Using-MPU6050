#  Air Mouse---Wireless-Mouse-Gesture-Control-Using-MPU6050-FreeRTOS 🐭🖱️🫱💻

## Introduction
This project is a gesture-controlled wireless mouse using the MPU6050 sensor. The system collects motion data from the user’s hand, processes it, and transmits signals via Bluetooth to control the mouse on a computer.

## Features
- Uses an MPU6050 sensor to track hand movement.
- Integrates a Flex sensor to detect finger bending for click actions.
- Communicates wirelessly via a Bluetooth HC-06 module.
- Implements FreeRTOS for real-time task management.
- Python script for interpreting signals and controlling the mouse.

## System Overview
### Block Diagram
![System Block Diagram](https://github.com/quynhphamduong/Air-Mouse---Wireless-Mouse-Gesture-Control-Using-MPU6050/blob/main/Block%20diagram.png)

### Components
- **Microcontroller**: STM32F103C8T6
- **Motion Sensor**: MPU6050 (Accelerometer + Gyroscope)
- **Wireless Communication**: Bluetooth HC-06
- **Flex Sensor**: Detects finger bending for clicks
- **Power Supply**: 5V/3A

## Hardware Design
### Circuit Diagram
![Circuit Diagram](https://github.com/quynhphamduong/Air-Mouse---Wireless-Mouse-Gesture-Control-Using-MPU6050/blob/77a11beec50cd34779986e8cda144eaf0a335076/Circuit%20diagram.jpg)

The hardware consists of an STM32F103C8T6 microcontroller interfacing with the MPU6050 sensor and the HC-06 Bluetooth module. The flex sensor is used to detect clicks.

## Software Design
The firmware is developed using FreeRTOS with multiple tasks managing data collection, processing, and communication.

### Task Breakdown
- **MPU6050 Data Collection**: Reads motion data via I2C.
- **Data Filtering**: Uses a complementary filter to smoothen noisy sensor data.
- **Bluetooth Transmission**: Sends processed data to the computer.
- **Python Mouse Control**: Interprets sensor data to control the mouse cursor.

### Software Flowchart
![Software Flowchart](https://github.com/quynhphamduong/Air-Mouse---Wireless-Mouse-Gesture-Control-Using-MPU6050/blob/77a11beec50cd34779986e8cda144eaf0a335076/main_flowchart.png)

## Python Script for Mouse Control
A Python script using the `pyautogui` library interprets Bluetooth signals and moves the cursor accordingly.

### Example Code Snippet
```python
import pyautogui
import serial

ser = serial.Serial('COM5', 9600)
while True:
    data = ser.readline().decode().strip()
    if data:
        x, y = map(int, data.split('/'))
        pyautogui.moveRel(x, y)
```

## Results and Evaluation
![GUI](https://github.com/quynhphamduong/Air-Mouse---Wireless-Mouse-Gesture-Control-Using-MPU6050/blob/77a11beec50cd34779986e8cda144eaf0a335076/guiguide.png)

### Advantages
- Intuitive and hands-free operation.
- Portable and flexible design.
- Customizable gestures for different tasks.

### Limitations
- Requires fine-tuning for precise motion control.
- Bluetooth latency may affect real-time performance.

## Installation and Usage
1. Upload the firmware to the STM32 microcontroller.
2. Pair the HC-06 module with the PC (module password : 1234).
3. Run the mouse.py to start mouse control.


## Contributors
- **Phạm Dương Quỳnh**
- **Đào Anh Phi**


## License
This project is open-source and available under the MIT License.

