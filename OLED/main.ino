// This file is part of Ford FG HVAC Display.

// Ford FG HVAC Display is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Ford FG HVAC Display is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Ford FG HVAC Display.  If not, see <https://www.gnu.org/licenses/>.

 
// Ford FG HVAC Display - CAN BUS receiving code
// 
// Heavily derived from MitchellH's source code available at https://fordforums.com.au/showthread.php?t=11430769
// 
// Created by Kyle May in 2019.

#include "mcp_can.h"
#include <SPI.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include "structures.h"
#include "layout.h"

MCP_CAN CAN(10);

// Vent Status
unsigned char ventCAN = 0b0;
// AC Temp
double acTempCAN = 0;
// Outside Temp
int outsideTempCAN = 0;
// Cabin temp
double cabinTempCAN = 100;
// Fan Speed
int fanSpeedCAN = 0;
// BCM Status
unsigned char bcmCAN = 0;
// Headlight status
unsigned char headlightsCAN = 0;

unsigned char buf[8];

// Variables to signal send message
byte Send764 = 0;
byte Send738 = 0;
byte Send1372 = 0;

//ACU Codes
byte Send742 = 0;
byte Send748 = 0;
byte Send751 = 0;
byte Send754 = 0;
byte Send756 = 0;
byte Send761 = 0;
byte Send777 = 0;
byte Send779 = 0;
byte Send781 = 0;
byte Send783 = 0;
byte Send785 = 0;
byte Send791 = 0;
byte Send1292 = 0;
byte Send1788 = 0;

SimpleTimer tmr100ms;

Layout *layout = new Layout();

void setup() {
    layout->setup();
    layout->draw_info_text("Loading");

    while(CAN_OK != CAN.begin(CAN_125KBPS, MCP_8MHz)) {
        delay(100);
        layout->draw_info_text("Failed to init CAN Bus");
    }
    layout->draw_info_text("LS CAN Bus Loaded");
    tmr100ms.setInterval(100, SetSend100ms);
}

void loop() {
    requestHVAC();
    tmr100ms.run();

    //Process CAN Data
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
        CAN.readMsgBuf(0, buf);

        //Read CAN Node ID
        int CANNodeID = CAN.getCanId();

        // HVAC
        if (CANNodeID == 851) {
            //Vent Status
            if (ventCAN != buf[0]) {
                ventCAN = buf[0];
                
                // Temperature Status
                if (acTempCAN != buf[3]) {
                    acTempCAN = buf[3];
                }
    
                // Outside Temp
                if (outsideTempCAN != buf[4]) {
                    outsideTempCAN = buf[4];
                }
    
                // Fan Speed
                if (fanSpeedCAN != buf[7]) {
                    fanSpeedCAN = buf[7];
                }
                
                // Draw
                draw();
            }
        } else if (CANNodeID == 1027) { // BCM
            if (bcmCAN != buf[0]) {
                bcmCAN = buf[0];
                // Draw
                draw();
             }
        } else if (CANNodeID == 787) { // Cabin temp
            if(cabinTempCAN != buf[0]) {
                cabinTempCAN = buf[0];
                // Draw
                draw();
            }
        } else if (CANNodeID == 296) { // Headlights
            if (headlightsCAN != buf[0]) {
                headlightsCAN = buf[0];
                // Lower brightness if headlights on
                if(headlightsCAN <= 0) {
                  layout->setContrast(255);
                } else {
                  layout->setContrast(80);
                }
            }
        }
    }
}

void draw() {
    State state;
    state.ventCAN = ventCAN;
    state.acTempCAN = acTempCAN;
    state.outsideTempCAN = outsideTempCAN;
    state.fanSpeedCAN = fanSpeedCAN;
    state.bcmCAN = bcmCAN;
    state.cabinTempCAN = cabinTempCAN;
    layout->draw(state);
}

// Requests HVAC info from BEM
void requestHVAC() {
    if(Send764 == 1) {
        code764function();
        Send764 = 0;
    }

    if(Send738 >= 2) {
        code738function();
        Send738 = 0;
    }
    
    
    if(Send1372 >= 5) {
        code1372function();
        Send1372 = 0;
    }

    if(Send742 >= 2) {
        code742function();
        Send742 = 0;
    }

    if(Send748 >= 2) {
        code748function();
        Send748 = 0;
    }

    if(Send751 >= 2) {
        code751function();
        Send751 = 0;
    }

    if(Send754 >= 2) {
        code754function();
        Send754 = 0;
    }

    if(Send756 >= 5) {
        code756function();
        Send756 = 0;
    }

    if(Send761 >= 5) {
        code761function();
        Send761 = 0;
    }

    if(Send783 >= 2) {
        code783function();
        Send783 = 0;
    }

    if(Send785 >= 2) {
        code785function();
        Send785 = 0;
    }

    if(Send777 >= 5) {
        code777function();
        Send777 = 0;
    }

    if(Send779 >= 5) {
        code779function();
        Send779 = 0;
    }

    if(Send781 >= 5) {
        code781function();
        Send781 = 0;
    }

    if(Send791 >= 5) {
        code791function();
        Send791 = 0;
    }

    if(Send1292 >= 5) {
        code1292function();
        Send1292 = 0;
    }

    if(Send1788 >= 5) {
        code1788function();
        Send1788 = 0;
    }
}

// Every 100ms updates timer increments for each function
void SetSend100ms() {
    Send764 = 1;
    //500ms
    Send1372++;
    //200ms
    Send738++;

    //ACU
    //200ms
    Send742++;
    Send748++;
    Send751++;
    Send754++;
    Send783++;
    Send785++;

    //500ms
    Send756++;
    Send761++;
    Send777++;
    Send779++;
    Send781++;
    Send1292++;
    Send791++;
    Send1788++;
}

// ICC Unit Data Requests:

//ACU CAN CODES
void code742function() {
    unsigned char char742[8] = {4, 227, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x2E6, 0, 8, char742);
}

void code748function() {
    unsigned char char748[8] = {2, 159, 3, 3, 3, 202, 0, 0};
    CAN.sendMsgBuf(0x2EC, 0, 8, char748);
}

void code751function() {
    unsigned char char751[8] = {4, 227, 6, 78, 8, 29, 0, 0};
    CAN.sendMsgBuf(0x2EF, 0, 8, char751);
}

void code754function() {
    unsigned char char754[8] = {4, 10, 13, 10, 9, 73, 72, 5};
    CAN.sendMsgBuf(0x2F2, 0, 8, char754);
}

void code756function() {
    unsigned char char756[8] = {0, 0, 255, 255, 255, 252, 0, 0};
    CAN.sendMsgBuf(0x2F4, 0, 8, char756);
}

void code761function() {
    unsigned char char761[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x2F9, 0, 8, char761);
}

void code1292function() {
    unsigned char char1292[8] = {17, 2, 110, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x50C, 0, 8, char1292);
}

void code1788function() {
    unsigned char char1788[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x6FC, 0, 8, char1788);
}

void code781function() {
    unsigned char char781[8] = {0, 0, 0, 1, 0, 22, 0, 0};
    CAN.sendMsgBuf(0x30D, 0, 8, char781);
}

void code791function() {
    unsigned char char791[8] = {87, 66, 68, 51, 48, 53, 50, 51};
    CAN.sendMsgBuf(0x317, 0, 8, char791);
}

void code738function() {
    unsigned char char738[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x2E2, 0, 8, char738);
}

void code764function() {
    unsigned char char764[8] = {0, 0, 1, 0, 31, 0, 2, 4};
    CAN.sendMsgBuf(0x2FC, 0, 8, char764);
}

void code783function() {
    unsigned char char783[8] = {31, 31, 31, 31, 31, 0, 0, 0};
    CAN.sendMsgBuf(0x30F, 0, 8, char783);
}

void code785function() {
    unsigned char char785[8] = {31, 148, 31, 31, 31, 31, 0, 33};
    CAN.sendMsgBuf(0x311, 0, 8, char785);
}

void code777function() {
    unsigned char char777[8] = {32, 32, 32, 32, 32, 32, 32, 32};
    CAN.sendMsgBuf(0x309, 0, 8, char777);
}

void code779function() {
    unsigned char char779[8] = {0, 0, 2, 0, 0, 3, 8, 0};
    CAN.sendMsgBuf(0x30B, 0, 8, char779);
}

void code1372function() {
    unsigned char char1372[8] = {1, 2, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x55C, 0, 8, char1372);
}