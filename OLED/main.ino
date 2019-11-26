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

Layout *layout = new Layout();

// Init can bus controller with CS pin on D9
MCP_CAN CAN(9);

// CAN Codes
const int HVAC_CAN = 851;
const int BCM_1_CAN = 1027;
const int CABIN_CAN = 787;
const int HEADLIGHTS_CAN = 296;
const int ICC_BUTTONS_CAN = 764;

// Vent Status
unsigned char ventCAN = 0b0;
// AC Temp
double acTempCAN = 0;
// Outside Temp
int outsideTempCAN = 0;
// Cabin temp
double cabinTempCAN = 0;
// Fan Speed
int fanSpeedCAN = 0;
// BCM Status
unsigned char bcmCAN = 0;
// Headlight status
unsigned char headlightsCAN = 0;

unsigned char buf[8];
bool firstRun = true;
bool drawing2 = false;

//variables to signal send message
byte Send738 = 0;
byte Send775 = 0;
byte Send787 = 0;
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

//TX CAN Codes
unsigned char char738[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char char775[8] = {0, 0, 0, 128, 0, 0, 0, 0};

unsigned char char787[8] = {148, 0, 0, 0, 0, 0, 0, 0};

int reset775 = 0;

SimpleTimer tmr100ms;

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(50);
    
    layout->setup();

    while (CAN_OK != CAN.begin(CAN_125KBPS)) {
        Serial.println("failed to connect");
        layout->draw_info_text("Failed to Connect");
        delay(100);
    }
    Serial.println("Connected");
    layout->draw_info_text("Connected");

    tmr100ms.setInterval(100, SetSend100ms);
}

void loop() {
    requestHVAC();
    tmr100ms.run();
    // Process CAN Data
    if (CAN_MSGAVAIL == CAN.checkReceive()) {
        CAN.readMsgBuf(0, buf);
        //Read CAN Node ID
        int CANNodeID = CAN.getCanId();
        // HVAC
        if (CANNodeID == HVAC_CAN) {
            // Vent Status
            if (ventCAN != buf[0]) {
                ventCAN = buf[0];
                draw();
            }

            // Temperature Status
            if (acTempCAN != buf[3]) {
                acTempCAN = buf[3];
                draw();
            }

            // Outside Temp
            if (outsideTempCAN != buf[4]) {
                outsideTempCAN = buf[4];
                draw();
            }

            // Fan Speed
            if (fanSpeedCAN != buf[7]) {
                fanSpeedCAN = buf[7];
                draw();
            }
        } else if (CANNodeID == BCM_1_CAN) { // BCM 1
            if (bcmCAN != buf[0]) {
                bcmCAN = buf[0];
                draw();
            }
        } else if (CANNodeID == CABIN_CAN) { // Cabin temp
            if (cabinTempCAN != buf[0]) {
                cabinTempCAN = buf[0];
                draw();
            }
        } else if (CANNodeID == HEADLIGHTS_CAN) { // Headlights
            if (headlightsCAN != buf[0]) {
                headlightsCAN = buf[0];
                // Lower brightness if headlights on
                if(headlightsCAN <= 0) {
                    layout->setContrast(255);
                } else {
                    layout->setContrast(80);
                }
            }
        } else if (CANNodeID == ICC_BUTTONS_CAN) { // ICC buttons
            // Detect press of 'Load' button to draw second page on screen
            if(buf[1] == 64) {
                draw2();
            } else {
                if(drawing2){ 
                    drawing2 = false;
                    draw();
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
    state.cabinTempCAN = cabinTempCAN;
    state.fanSpeedCAN = fanSpeedCAN;
    state.bcmCAN = bcmCAN;
    if(drawing2) {
        layout->draw2(state);
    } else {
        layout->draw(state);
    }
}

void draw2() {
    drawing2 = true;
    draw();
}

// Requests HVAC info from BEM
void requestHVAC() {

    if (Send738 >= 2 || firstRun) {
        code738function();
        Send738 = 0;
    }

    if (Send775 >= 5 || firstRun) {
        // Sets car settings, don't need this.
        // code775function();
        Send775 = 0;
    }

    if (Send1372 >= 5 || firstRun) {
        code1372function();
        Send1372 = 0;
    }

    if (Send742 >= 2 || firstRun) {
        code742function();
        Send742 = 0;
    }

    if (Send748 >= 2 || firstRun) {
        code748function();
        Send748 = 0;
    }

    if (Send751 >= 2 || firstRun) {
        code751function();
        Send751 = 0;
    }

    if (Send754 >= 2 || firstRun) {
        code754function();
        Send754 = 0;
    }

    if (Send756 >= 5 || firstRun) {
        code756function();
        Send756 = 0;
    }

    if (Send761 >= 5 || firstRun) {
        code761function();
        Send761 = 0;
    }

    if (Send783 >= 2 || firstRun) {
        code783function();
        Send783 = 0;
    }

    if (Send785 >= 2 || firstRun) {
        code785function();
        Send785 = 0;
    }

    if (Send777 >= 5 || firstRun) {
        code777function();
        Send777 = 0;
    }

    if (Send779 >= 5 || firstRun) {
        code779function();
        Send779 = 0;
    }

    if (Send781 >= 5 || firstRun) {
        code781function();
        Send781 = 0;
    }

    if (Send791 >= 5 || firstRun) {
        code791function();
        Send791 = 0;
    }

    if (Send1292 >= 5 || firstRun) {
        code1292function();
        Send1292 = 0;
    }
    //lowered from 10 to 5
    if (Send1788 >= 5 || firstRun) {
        code1788function();
        Send1788 = 0;
    }
    if (firstRun) {
        firstRun = false;
    }
}

// Every 100ms updates timer increments for each function
void SetSend100ms() {

    //200ms
    Send742++;
    Send748++;
    Send751++;
    Send754++;
    Send783++;
    Send785++;
    Send738++;
    Send787++;

    //500ms
    Send756++;
    Send761++;
    Send777++;
    Send779++;
    Send781++;
    Send1292++;
    Send775++;
    Send1372++;
    Send791++;
    Send1788++;
}

// ICC Unit Data Requests:

void code748function()
{
    unsigned char char748[8] = {2, 159, 3, 3, 3, 202, 0, 0};
    CAN.sendMsgBuf(0x2EC, 0, 8, char748);

}
void code754function()
{
    unsigned char char754[8] = {4, 10, 13, 10, 9, 73, 72, 5};
    CAN.sendMsgBuf(0x2F2, 0, 8, char754);
}

// ACU - Unknown

void code742function() {
    unsigned char char742[8] = {4, 227, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x2E6, 0, 8, char742);
}

void code751function()
{
    unsigned char char751[8] = {4, 227, 6, 78, 8, 29, 0, 0};
    CAN.sendMsgBuf(0x2EF, 0, 8, char751);
}

void code756function()
{
    unsigned char char756[8] = {0, 0, 255, 255, 255, 252, 0, 0};
    CAN.sendMsgBuf(0x2F4, 0, 8, char756);
}

void code761function()
{
    unsigned char char761[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x2F9, 0, 8, char761);
}

void code1292function()
{
    unsigned char char1292[8] = {17, 2, 110, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x50C, 0, 8, char1292);
}

void code1788function()
{
    unsigned char char1788[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x6FC, 0, 8, char1788);
}

void code781function()
{
    unsigned char char781[8] = {0, 0, 0, 1, 0, 22, 0, 0};
    CAN.sendMsgBuf(0x30D, 0, 8, char781);
}

void code791function()
{
    unsigned char char791[8] = {87, 66, 68, 51, 48, 53, 50, 51};
    CAN.sendMsgBuf(0x317, 0, 8, char791);
}


//ICC SCREEN CAN CODES

void code738function()
{
    CAN.sendMsgBuf(0x2E2, 0, 8, char738);
}


void code783function()
{
    unsigned char char783[8] = {31, 31, 31, 31, 31, 0, 0, 0};
    CAN.sendMsgBuf(0x30F, 0, 8, char783);
}

void code785function()
{
    unsigned char char785[8] = {31, 148, 31, 31, 31, 31, 0, 33};
    CAN.sendMsgBuf(0x311, 0, 8, char785);
}

void code777function()
{
    unsigned char char777[8] = {32, 32, 32, 32, 32, 32, 32, 32};
    CAN.sendMsgBuf(0x309, 0, 8, char777);
}

void code779function()
{
    unsigned char char779[8] = {0, 0, 2, 0, 0, 3, 8, 0};
    CAN.sendMsgBuf(0x30B, 0, 8, char779);
}


void code775function()
{
    CAN.sendMsgBuf(0x307, 0, 8, char775);
    if (reset775 == 1)
    {
        char775[0] = 0;
        char775[1] = 0;
        char775[2] = 0;
        char775[3] = 128;
        char775[4] = 0;
        char775[5] = 0;
        char775[6] = 0;
        char775[7] = 0;
        reset775 = 0;
    }
}


void code1372function()
{
    unsigned char char1372[8] = {1, 2, 0, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(0x55C, 0, 8, char1372);
}



