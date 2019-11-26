// This file is part of Ford FG HVAC Display.

// Ford FG HVAC Display is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Ford FG HVAC Display is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Ford FG HVAC Display.    If not, see <https://www.gnu.org/licenses/>.


// Ford FG HVAC Display - OLED Layout Code
// 
// Created by Kyle May in 2019.

#include <U8g2lib.h>
#include "images.h"
#include "structures.h"
#include "layout.h"

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C oled(U8G2_R0); 
const int offset = 15;

void Layout::setup() {
    oled.begin();
    this->draw_info_text("Ford FG");
}

void Layout::draw_info_text(char* text) {
    oled.firstPage();
    do {
        oled.setFont(u8g2_font_t0_12b_tf);
        double w = oled.getStrWidth(text);
        double x = (128.0 - w) / 2.0;
        oled.drawStr(x + offset, 0, text);
    } while(oled.nextPage());
}

void Layout::draw2(State state) {
    // draw a second page for supplementary info on smaller screen
    oled.firstPage();
    do {
        double cabin = (state.cabinTempCAN - 80) / 2;
        String cabin_temp_str = "Cabin " + String(cabin, 1) + "\xB0";
        oled.setFont(u8g2_font_helvB12_tf);
        oled.drawStr(2 + offset, 18, cabin_temp_str.c_str());
    } while(oled.nextPage());
}

void Layout::draw(State state) {
    // Not enough memory on Arduino Uno to store the entire buffer in memory
    oled.firstPage();
    do {
        // Outside temperature icon
        oled.setFont(u8g2_font_open_iconic_weather_1x_t);
        oled.drawGlyph(offset + 48, 10, sun_icon);

        // Outside temperature
        String outside_temp_str = String(state.outsideTempCAN) + "\xB0";
        oled.setFont(u8g2_font_t0_13b_tf);
        oled.drawStr(60 + offset,10,outside_temp_str.c_str());

        // Requested temperature
        oled.setFont(u8g2_font_helvB12_tf);
        
        // Convert temp
        double acTemp = state.acTempCAN / 2.0;
        if(acTemp < 0.5) {
            oled.drawStr(0 + offset,31,"Off");
        } else if(acTemp == 0.5) {
            oled.drawStr(0 + offset,31,"Low");
        } else {
            String requested_temp_str = String(acTemp, 1) + "\xB0";
            oled.drawStr(0 + offset,31,requested_temp_str.c_str());
        }

        // Fan
        this->draw_fan(state.fanSpeedCAN);

        // Vent
        this->draw_vent(state.ventCAN);

        // BCM
        this->draw_bcm(state.bcmCAN);

        // Cabin temp
        double cabin = (state.cabinTempCAN - 80) / 2;
        String cabin_temp_str = "Cabin " + String(cabin, 1) + "\xB0";
        oled.setFont(u8g2_font_helvB08_tf);
        oled.drawStr(0 + offset,62,cabin_temp_str.c_str());
    } while(oled.nextPage());
}

void Layout::draw_bcm(unsigned char bcm) {
    // Third bit is rear demister status
    if(bitRead(bcm, 2) == 1) {
        oled.setFont(u8g2_font_t0_12b_tf);
        oled.drawStr(30 + offset,10,"RD");
    }
}

void Layout::draw_fan(int speed) {
    // Bit shift to get rid of irrelevant fan info
    int fan_speed = speed & ((1 << 4) - 1);
    bool isAuto = fan_speed == 0;

    // Fan icon
    oled.drawXBMP(46 + offset, 18, 14, 14, fan);
    oled.setFont(u8g2_font_helvB12_tf);

    // Fan speed
    if(!isAuto) {
        oled.drawStr(64 + offset,31,String(fan_speed).c_str());
    } else {
        oled.drawStr(64 + offset,31,"A");
    }
}

void Layout::setContrast(uint8_t value) {
    oled.setContrast(value);
}

void Layout::draw_vent(unsigned char ventRaw) {
    bool demister_on = false;
    bool face_on = false;
    bool foot_on = false;
    bool ac_on = false;

    Vent vent = Vent::automatic;
    Trinary recirculation = Trinary::au;

    // AC
    ac_on = bitRead(ventRaw, 7) == 0;

    // Front demister
    demister_on = bitRead(ventRaw, 2) == 1;

    // Foot
    foot_on = bitRead(ventRaw, 3) == 1;

    // Face
    face_on = bitRead(ventRaw, 4) == 1;

    // Recirculation, auto if b == 0 and c == 0. b = on, c = off
    if (bitRead(ventRaw, 5) == 0 && bitRead(ventRaw, 6) == 0) {
        recirculation = Trinary::au;
    } else if (bitRead(ventRaw, 6) == 1) {
        recirculation = Trinary::on;
    } else if (bitRead(ventRaw, 5) == 1) {
        recirculation = Trinary::off;
    }

    // Update layout
    if (foot_on && face_on) {
        vent = Vent::face_foot;
    } else if(foot_on && demister_on) {
        vent = Vent::foot_demister;
    } else if(foot_on) {
        vent = Vent::foot;
    } else if(demister_on) {
        vent = Vent::demister;
    } else if(face_on) {
        vent = Vent::face;
    } else {
        vent = Vent::automatic;
    }

    // Vent
    const int pos = 80 + offset; // Draw position
    const int image_size = 32;
    switch(vent) {
        case Vent::automatic:
        oled.drawXBMP(pos, 0, image_size, image_size, pos_auto_icon);
        break;
        case Vent::face:
        oled.drawXBMP(pos, 0, image_size, image_size, pos_face_icon);
        break;
        case Vent::foot:
        oled.drawXBMP(pos, 0, image_size, image_size, pos_floor_icon);
        break;
        case Vent::demister:
        oled.drawXBMP(pos, 0, image_size, image_size, pos_screen_icon);
        break;
        case Vent::face_foot:
        oled.drawXBMP(pos, 0, image_size, image_size, pos_face_floor_icon);
        break;
        case Vent::foot_demister:
        oled.drawXBMP(pos, 0, image_size, image_size, pos_floor_screen_icon);
        break;
    }

    // AC status (icon)
    if(ac_on) {
        oled.drawXBMP(0 + offset, 0, 12, 12, ac_icon);
    }

    // Recirculation status
    oled.setFont(u8g2_font_open_iconic_arrow_1x_t);
    if(recirculation == Trinary::on) {
        oled.drawGlyph(offset + 16, 10, recirc_on_icon);
    } else if(recirculation == Trinary::off) {
        oled.drawGlyph(offset + 16, 10, recirc_off_icon);
    } // else, recirc auto - no icon
    
}