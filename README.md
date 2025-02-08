#  Air Mouse---Wireless-Mouse-Gesture-Control-Using-MPU6050 
🐭🖱️🫱💻
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
![System Block Diagram](https://github.com/quynhphamduong/Air-Mouse---Wireless-Mouse-Gesture-Control-Using-MPU6050/blob/0a358edcf26ce7ff9b889049cd9a96d1d3186624/Block%20diagram.png)

### Components
- **Microcontroller**: STM32F103C8T6
- **Motion Sensor**: MPU6050 (Accelerometer + Gyroscope)
- **Wireless Communication**: Bluetooth HC-06
- **Flex Sensor**: Detects finger bending for clicks
- **Power Supply**: 5V/3A

## Hardware Design
### Circuit Diagram
![Circuit Diagram](images/circuit_diagram.png)

The hardware consists of an STM32F103C8T6 microcontroller interfacing with the MPU6050 sensor and the HC-06 Bluetooth module. The flex sensor is used to detect clicks.

## Software Design
The firmware is developed using FreeRTOS with multiple tasks managing data collection, processing, and communication.

### Task Breakdown
- **MPU6050 Data Collection**: Reads motion data via I2C.
- **Data Filtering**: Uses a complementary filter to smoothen noisy sensor data.
- **Bluetooth Transmission**: Sends processed data to the computer.
- **Python Mouse Control**: Interprets sensor data to control the mouse cursor.

### Software Flowchart
![Software Flowchart](images/software_flowchart.png)

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
### Prototype Hardware
![Prototype](images/prototype.jpg)

### Advantages
- Intuitive and hands-free operation.
- Portable and flexible design.
- Customizable gestures for different tasks.

### Limitations
- Requires fine-tuning for precise motion control.
- Bluetooth latency may affect real-time performance.

## Installation and Usage
1. Upload the firmware to the STM32 microcontroller.
2. Pair the HC-06 module with the PC.
3. Run the Python script to start mouse control.

## Repository
For full source code and schematics, visit: [GitHub Repository](https://github.com/Anh-phi-ngu-IT-Dien-tu/Nhom_31_lap_trinh_nhung.git)

## Contributors
- **Phạm Dương Quỳnh** - 2114620
- **Đào Anh Phi** - 2111996


## License
This project is open-source and available under the MIT License.

