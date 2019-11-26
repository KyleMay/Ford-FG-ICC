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


// Ford FG HVAC Display - Data structures code
// 
// Created by Kyle May in 2019.

#ifndef STRUCTURES_H
#define STRUCTURES_H

struct State {
    unsigned char ventCAN;
    double acTempCAN;
    double cabinTempCAN;
    int outsideTempCAN;
    int fanSpeedCAN;
    unsigned char bcmCAN;
};

enum Vent {
    automatic = 1,
    face,
    foot,
    demister,
    face_foot,
    foot_demister
};

enum Trinary {
    au = 1,
    on,
    off
};

#endif