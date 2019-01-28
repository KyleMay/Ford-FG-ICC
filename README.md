# Ford FG HVAC Display

Information available at https://fordforums.com.au/showthread.php?t=11475851, influenced by https://fordforums.com.au/showthread.php?t=11430769.

**Note: This code is under the GPLv3 license. Any modifications must have the source code distributed under this license.**

# Files

## PythonGUI
This is the Python GUI (Kivy) version, where an Arduino would be used to read the CAN data and send it over serial to a 
Raspberry Pi or similar, where the Kivy app would display the information on the head unit's screen.


### Arduino
- `CANReader.ino`: Reads the HVAC information from the CAN BUS and sends it over serial.

### Python
- `HVACDisplay.py`: Receives the HVAC information from the Arduino and displays it on the GUI.
- `HVACDisplay.kv`: The UI markup for the Python app.

## OLED
This is the OLED screen version, all run on an Arduino and using an MCP2515 module.

- `main.ino`: Reads the HVAC information from the CAN BUS and passes it on to the Layout class
- `layout.h/layout.ino`: The layout class displays the information received from the CAN BUS on to an external OLED display, using the [U8G2](https://github.com/olikraus/u8g2) library.
- `images.h`: Bitmap arrays for the images to be displayed
- `structures.h`: Data structures used for HVAC state