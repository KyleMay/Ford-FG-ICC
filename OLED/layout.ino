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

 
// Ford FG HVAC Display - OLED Layout Code
// 
// Created by Kyle May in 2019.

#include <U8g2lib.h>
#include "images.h"
#include "structures.h"
#include "layout.h"

U8G2_SH1106_128X64_NONAME_2_HW_I2C oled(U8G2_R0); 

void Layout::setup() {
    oled.begin();
    this->draw_info_text("Layout Started");
}

void Layout::draw_info_text(char* text) {
    oled.clearBuffer();
    oled.setFont(u8g2_font_t0_12b_tf);
    oled.drawStr(0, 12, text);
    oled.sendBuffer();
}

void Layout::draw(State state) {
	// Not enough memory on Arduino Uno to store the entire buffer in memory
	oled.firstPage();
	do {
        // Outside temperature icon
        oled.setFont(u8g2_font_open_iconic_weather_1x_t);
        oled.drawGlyph(48, 10, sun_icon);

		// Outside temperature
        String outside_temp_str = String(state.outsideTempCAN) + "\xB0";
        oled.setFont(u8g2_font_t0_12b_tf);
        oled.drawStr(60,10,outside_temp_str.c_str());

        // Requested temperature
        oled.setFont(u8g2_font_helvB12_tf);
        // Convert temp
        double acTemp = (double)state.acTempCAN[3] / 2.0;
        if(acTemp == 0.5) {
        	oled.drawStr(0,31,"Min""\xB0");
        } else {
			String requested_temp_str = String(acTemp, 1) + "\xB0";
			oled.drawStr(0,31,requested_temp_str.c_str());
        }

        // Fan
        this->draw_fan(state.fanSpeedCAN);

        // Vent
        this->draw_vent(state.ventCAN);
        
        // BCM
        this->draw_bcm(state.bcmCAN);
        
        // Cabin temp
        double cabin = (state.cabinTempCAN - 100) / 2;
        String cabin_temp_str = "Cabin " + String(cabin, 1) + "\xB0";
        oled.setFont(u8g2_font_helvB08_tf);
        oled.drawStr(0,62,cabin_temp_str.c_str());
	} while(oled.nextPage());
}

void Layout::draw_bcm(unsigned char bcm) {
	// Third bit is rear demister status
	if(bitRead(bcm, 3) == 1) {
		oled.setFont(u8g2_font_t0_12b_tf);
        oled.drawStr(30,10,"RD")
	}
}

void Layout::draw_fan(int speed) {
	// Set to display auto mode by default
    int fan_speed = -1;

    // If over 144 that ain't right
    if(speed >= 144) {
        speed -= 144;
    }
    
    // If less than 128 then we are not in auto mode
    if(speed < 128) {
        fan_speed = speed;
    }

    // Fan icon
    oled.drawXBMP(46, 18, 14, 14, fan);
    oled.setFont(u8g2_font_helvB12_tf);
    
    // Fan speed
    if(fan_speed >= 0) {
        oled.drawStr(64,31,String(fan_speed).c_str());
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
    const int pos = 80; // Draw position
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
        oled.drawXBMP(0, 0, 12, 12, ac_icon);
    }

    // Recirculation status
    oled.setFont(u8g2_font_open_iconic_arrow_1x_t);
    if(recirculation == Trinary::on) {
        oled.drawGlyph(16, 10, recirc_on_icon);
    } else if(recirculation == Trinary::off) {
        oled.drawGlyph(16, 10, recirc_off_icon);
    } // else, recirc auto - no icon
}