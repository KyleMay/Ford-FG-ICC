# Ford FG HVAC Display

Information available at https://fordforums.com.au/showthread.php?t=11475851, influenced by https://fordforums.com.au/showthread.php?t=11430769.

# Files

## Arduino
- `CANReader.ino`: Reads the HVAC information from the CAN BUS and sends it over serial.

## Python
- `HVACDisplay.py`: Receives the HVAC information from the Arduino and displays it on the GUI.
- `HVACDisplayUI.kv`: The UI markup for the Python app.
