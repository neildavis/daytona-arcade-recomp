#include "runtime/hud.h"

#include <cstdlib>

namespace rt {

void hook_hud_time(Cpu &c) {
    uint32_t new_time =c.bus->read_dword(0x5010d0) >> 6;
    if (new_time != Hud::hud_time) {
        Hud::hud_time = new_time;
        /*
        if (new_time & 0xffff0000) {
            std::printf("HUD: Time=NaN\n");
        } else {
            std::printf("HUD: Time=%02d\n", new_time);
        }
        */
    }
}

void hook_hud_speed(Cpu&c) {
    uint32_t new_speed =c.bus->read_dword(0x504e18);
    if (new_speed != Hud::hud_speed) {
        Hud::hud_speed = new_speed;
        //std::printf("HUD: Speed=%03d km/h\n", new_speed);
    }
}

void hook_hud_pos(Cpu&c) {
    uint16_t new_pos =c.bus->read_word(0x504e6a);
    if (new_pos != Hud::hud_pos) {
        Hud::hud_pos = new_pos;
        // pos is zero indexed so 0 == 1st, ... 39 == 40th
        //std::printf("HUD: Pos=%02d/40\n", new_pos + 1);
    }
}

void hook_hud_gear(Cpu&c) {
    uint32_t new_gear =c.bus->read_dword(0x504f48);
    if (new_gear != Hud::hud_gear) {
        Hud::hud_gear = new_gear;
        // Gear is zero indexed so 0..3 -> 1-4
        //std::printf("HUD: Gear=%d\n", new_gear + 1);
    }
}

void hook_hud_rpm(Cpu&c) {
    uint32_t new_rpm =c.bus->read_dword(0x504f54);
    if (new_rpm != Hud::hud_rpm) {
        Hud::hud_rpm = new_rpm;
        //std::printf("HUD: Revs=%04d rpm\n", new_rpm);
    }
}

void hook_hud_lap_cur(Cpu&c) {
    uint32_t new_lap =c.bus->read_dword(0x5010e0);
    if (new_lap != Hud::hud_lap_cur) {
        Hud::hud_lap_cur = new_lap;
        //std::printf("HUD: Lap=%d/%d\n", new_lap, Hud::hud_lap_tot);
    }
}

void hook_hud_lap_tot(Cpu&c) {
    uint32_t new_lap =c.bus->read_dword(0x5010ec);
    if (new_lap != Hud::hud_lap_tot) {
        Hud::hud_lap_tot = new_lap;
        //std::printf("HUD: Total Laps=%d\n", new_lap);
    }
}


} // namespace rt
