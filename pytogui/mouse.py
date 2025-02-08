# mouse.py
#Run this file will run GUI.py too

import pyautogui  # for controlling the mouse and keyboard
import serial     # for serial communication
import time
import threading  # for creating and managing threads
from multiprocessing import Manager, Process

# Variables to store the speed of the mouse movement
xspeed = 0
yspeed = 0

# Shared variables for speed multipliers
package = []
cleaned_data = []

"""
This function runs parallel with the main loop (using multithreading).
Based on the frame received from the MCU, we move or drag the mouse or stay at the current position.
"""
def mouse_thread(shared_values):
    global xspeed
    global yspeed
    global cleaned_data
    while True:
        try:
            x_multiplier = shared_values['x_multiplier']
            y_multiplier = shared_values['y_multiplier']

            if cleaned_data[0] == 'm':
                pyautogui.moveRel(xOffset=xspeed * x_multiplier, yOffset=yspeed * y_multiplier)
            elif cleaned_data[0] == 'd':
                pyautogui.dragRel(xOffset=xspeed * x_multiplier, yOffset=yspeed * y_multiplier)
        except Exception as e:
            print(f"Error in mouse_thread: {e}")
        print(pyautogui.position())

def update_speed(roll, pitch, shared_values):
    global xspeed, yspeed
    x_multiplier = shared_values['x_multiplier']
    y_multiplier = shared_values['y_multiplier']

    if roll < -15:
        xspeed = -30 * x_multiplier
    elif roll > 15:
        xspeed = 30 * x_multiplier
    else:
        xspeed = 0

    if pitch > 15:
        yspeed = 30 * y_multiplier
    elif pitch < -15:
        yspeed = -30 * y_multiplier
    else:
        yspeed = 0

def serial_read(shared_values):
    """
    To handle serial communication and update cleaned_data.
    """
    global cleaned_data
    while True:
        try:
            ser = serial.Serial("COM5", baudrate=9600, timeout=1)
            while ser.is_open:
                data = ser.readline().decode('utf-8')
                package = data.split('/')
                cleaned_data = [item.replace('\x00', '').strip() for item in package]
                print(cleaned_data)

                roll = int(cleaned_data[1])
                pitch = int(cleaned_data[2])

                update_speed(roll, pitch, shared_values)
        except Exception as e:
            print(f"Error in serial_read_thread: {e}")
            time.sleep(1)  # Retry after 1 second

if __name__ == "__main__":
    with Manager() as manager:
        # Shared dictionary to store x_multiplier and y_multiplier
        shared_values = manager.dict({'x_multiplier': 1, 'y_multiplier': 1})

        # Import GUI process from GUI.py (multiprocessing)
        from GUI import gui_process
        gui_process_instance = Process(target=gui_process, args=(shared_values,))
        gui_process_instance.start()

        # Start mouse thread
        thread1 = threading.Thread(target=mouse_thread, args=(shared_values,), daemon=True)
        thread1.start()

        # Start serial read thread
        serial_read(shared_values)

        # Wait for GUI process to finish
        gui_process_instance.join()
