# This file is part of Ford FG HVAC Display.

# Ford FG HVAC Display is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# Ford FG HVAC Display is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with Ford FG HVAC Display.  If not, see <https://www.gnu.org/licenses/>.

 
# Ford FG HVAC Display - HVAC UI Code
# 
# Created by Kyle May in 2018.


# -*- coding: utf-8 -*-

import os
os.environ['KIVY_GL_BACKEND'] = 'gl'
from kivy.app import App
from kivy.uix.button import Label
from kivy.uix.boxlayout import BoxLayout
from kivy.properties import StringProperty
from kivy.config import Config
from kivy.clock import mainthread
from kivy.core.window import Window
from enum import Enum
import time
import serial
import threading

Config.set('graphics', 'fullscreen', 'auto')
Config.write()

class Vent(Enum):
    auto = 1
    face = 2
    foot = 3
    demister = 4
    face_foot = 5
    foot_demister = 6
    
class Trinary(Enum):
    auto = 1
    on = 2
    off = 3

class MyLayout(BoxLayout):
    outside_temperature = 0
    requested_temperature = 0
    fan_speed = 10

    auto = False
    vent = Vent.auto
    recirculation = Trinary.auto
    ac_on = False
    rear_demist_on = False

    vent_source = StringProperty()
    recirculation_source = StringProperty()
    rear_demist_source = StringProperty()
    ac_source = StringProperty()
    requested_temperature_source = StringProperty()
    requested_temperature_string = StringProperty()
    fan_speed_source = StringProperty()
    time = StringProperty()
    outside_temperature_string = StringProperty()
    auto_string = StringProperty()
    
    @mainthread
    def update_layout(self):
        self.update_vent()
        self.update_recirculation()
        self.update_rear_demist()
        self.update_ac()
        self.update_requested_temperature()
        self.update_fan_speed()
        self.update_time()
        self.update_outside_temperature()
        self.update_auto()
    
    @mainthread
    def update_vent(self):
        if self.vent == Vent.auto:
            self.vent_source = 'img/pos_auto.png'
        elif self.vent == Vent.face:
            self.vent_source = 'img/pos_face.png'
        elif self.vent == Vent.foot:
            self.vent_source = 'img/pos_floor.png'
        elif self.vent == Vent.demister:
            self.vent_source = 'img/screen.png'
        elif self.vent == Vent.face_foot:
            self.vent_source = 'img/pos_face_floor.png'
        elif self.vent == Vent.foot_demister:
            self.vent_source = 'img/pos_floor_screen.png'

    def update_recirculation(self):
        if self.recirculation == Trinary.auto:
            self.recirculation_source = 'img/recirculation_auto.png'
        elif self.recirculation == Trinary.on:
            self.recirculation_source = 'img/recirculation_on.png'
        elif self.recirculation == Trinary.off:
            self.recirculation_source = 'img/recirculation_off.png'

    def update_rear_demist(self):
        if self.rear_demist_on:
            self.rear_demist_source = 'img/rear_demist_on.png'
        else:
            self.rear_demist_source = 'img/rear_demist_off.png'

    def update_ac(self):
        if self.ac_on:
            self.ac_source = 'img/air_con_on.png'
        else:
            self.ac_source = 'img/air_con_off.png'

    def update_requested_temperature(self):
        self.requested_temperature_string = str(self.requested_temperature) + "º"

        if self.requested_temperature < 20:
            self.requested_temperature_source = 'img/temperature_cool.png'
        elif self.requested_temperature > 24:
            self.requested_temperature_source = 'img/temperature_hot.png'
        else:
            self.requested_temperature_source = 'img/temperature_warm.png'

    def update_fan_speed(self):
        if self.fan_speed == 0:
            self.fan_speed_source = 'img/fan_auto.png'
        elif self.fan_speed == -1:
            self.fan_speed_source = 'img/fan_0.png'
        elif self.fan_speed == 1:
            self.fan_speed_source = 'img/fan_1.png'
        elif self.fan_speed == 2:
            self.fan_speed_source = 'img/fan_2.png'
        elif self.fan_speed == 3:
            self.fan_speed_source = 'img/fan_3.png'
        elif self.fan_speed == 4:
            self.fan_speed_source = 'img/fan_4.png'
        elif self.fan_speed == 5:
            self.fan_speed_source = 'img/fan_5.png'
        elif self.fan_speed == 6:
            self.fan_speed_source = 'img/fan_6.png'
        elif self.fan_speed == 7:
            self.fan_speed_source = 'img/fan_7.png'
        elif self.fan_speed == 8:
            self.fan_speed_source = 'img/fan_8.png'
        elif self.fan_speed == 9:
            self.fan_speed_source = 'img/fan_9.png'
        elif self.fan_speed == 10:
            self.fan_speed_source = 'img/fan_10.png'
        
    def update_time(self):
        self.time = time.strftime('%H:%M')
        
    def update_outside_temperature(self):
        self.outside_temperature_string = "Outside: " + str(self.outside_temperature) + "º"

    def update_auto(self):
        self.auto_string = "Auto" if self.auto else "Semi-auto"


class HVACApp(App):

    def build(self):
        self.layout = MyLayout()
        return self.layout

    def on_start(self):
        self.layout.requested_temperature = 0
        self.layout.vent = Vent.face_foot
        self.layout.recirculation = Trinary.off
        self.layout.ac_on = False
        self.layout.rear_demist_on = False
        self.layout.fan_speed = 0
        self.layout.outside_temperature = 0
        self.layout.update_layout()
        cr = CANReader()
        cr.layout = self.layout
        threading.Thread(target=cr.setup, args=()).start()

class CANReader():

    #Vent Status
    ventCAN = 0
    #AC Temp
    acTempCAN = 0
    #Outside Temp
    outsideTempCAN = 0
    #Fan Speed
    fanSpeedCAN = 0
    
    def setup(self):
        # TODO: Ensure this is the correct serial port
        ser = serial.Serial('/dev/ttyACM0', 115200)
        ser.write("SENDALLDATA")
        self.loop()
  
    def loop(self):
        while True:
            if(ser.in_waiting > 0):
                line = ser.readline()
                splitLine = line.split(":")

                # Ignore if not 4 segments
                if len(splitLine) < 4:
                    continue

                # Get key and value
                key = splitLine[1]
                value = splitLine[2]

                # Assign to CAN properties
                if key == "VENT":
                    ventCAN = value
                elif key == "ACTEMP":
                    acTempCAN = value
                elif key == "OUTSIDETEMP":
                    outsideTempCAN = value
                elif key == "FANSPEED":
                    fanSpeedCAN = value
                elif key == "ERROR" or key == "INFO":
                    print(value)
                else:
                    print("Unknown key: " + key + " with value: " + value)

    # Can't get this to work :(
    # def update_rear_demister_status(self):
    #     self.layout.rear_demist_on = self.code1027X1 >= 4
    #     self.layout.update_layout()
        
    def update_fan_status(self):
        fanspeedcode = self.fanSpeedCAN

        auto_fan = False

        if fanspeedcode >= 144:
            fanspeedcode -= 144
            
        if fanspeedcode >= 128:
            # Above 128 = Automatic Fan Speed
            auto_fan = True
            fanspeedcode = fanspeedcode - 128
        
        self.layout.fan_speed = -1 if auto_fan else fanspeedcode
        self.layout.update_layout()

    def update_temperature_status(self):
        self.layout.requested_temperature = self.acTempCAN
        self.layout.update_layout()

    def update_outside_temperature_status(self):
        self.layout.outside_temperature = self.outsideTempCAN
        self.layout.update_layout()

    def update_vent_status(self):
        demister_on = False
        foot_on = False
        face_on = False
        ac_on = False
        recirculation = Trinary.auto

        # AC
        ac_on = get_bit(self.ventCAN, 7) == 0

        # Front demister
        demister_on = get_bit(self.ventCAN, 2) == 1

        # Foot
        foot_on = get_bit(self.ventCAN, 3) == 1

        # Face
        face_on = get_bit(self.ventCAN, 4) == 1

        # Recirculation, auto if b == 0 and c == 0. b = on, c = off
        if get_bit(self.ventCAN, 5) == 0 and get_bit(self.ventCAN, 6) == 0:
            recirculation = Trinary.auto
        if get_bit(self.ventCAN, 6) == 1:
            recirculation = Trinary.on
        elif get_bit(self.ventCAN, 5) == 1:
            recirculation = Trinary.off


        # Update layout
        if foot_on and face_on:
            self.layout.vent = Vent.face_foot
        elif foot_on and demister_on:
            self.layout.vent = Vent.foot_demister
        elif foot_on:
            self.layout.vent = Vent.foot
        elif demister_on:
            self.layout.vent = Vent.demister        
        elif face_on:
            self.layout.vent = Vent.face
        else:
            self.layout.vent = Vent.auto

        self.layout.ac_on = ac_on     
        self.layout.recirculation = recirculation
                
        self.layout.update_layout()        
        
def get_bit(byteval,idx):
    return int(((byteval&(1<<idx))!=0))

if __name__ == "__main__":
    Window.size = (600, 480)
    Window.fullscreen = True
    app = HVACApp()
    app.run()
