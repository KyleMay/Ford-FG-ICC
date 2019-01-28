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

#ifndef LAYOUT_H
#define LAYOUT_H

class Layout {
private:
    const char recirc_on_icon = 0x56;
    const char recirc_off_icon = 0x5B;
    const char sun_icon = 0x41;
    
    void draw_bcm(unsigned char bcm);
    void draw_vent(unsigned char ventRaw);
    void draw_fan(int speed);
public:
    void setup();
    void draw(State state);
    void draw_info_text(char* text);
    void setContrast(uint8_t value);
};

#endif