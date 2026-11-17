#include "runtime/hud.h"
#include "runtime/game_revision.h"

#include <cstdlib>

namespace rt {

namespace {
constexpr uint32_t kHudTimeAddr     =  0x5010d0;
constexpr uint32_t kHudSpeedAddr    = kIsRevisionA ? 0x505198 : 0x504e18;
constexpr uint32_t kHudPosAddr      = kIsRevisionA ? 0x5051ec : 0x504e6a;
constexpr uint32_t kHudGearAddr     = kIsRevisionA ? 0x5052cc : 0x504f48;
constexpr uint32_t kHudRevsAddr     = kIsRevisionA ? 0x5052d8 : 0x504f54;
constexpr uint32_t kHudLapCurAddr   =  0x5010e0;
constexpr uint32_t kHudLapTotAddr   =  0x5010ec;
}

void hook_hud_time(Cpu &c) {
    uint32_t new_time =c.bus->read_dword(kHudTimeAddr) >> 6;
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
    uint32_t new_speed =c.bus->read_dword(kHudSpeedAddr);
    if (new_speed != Hud::hud_speed) {
        Hud::hud_speed = new_speed;
        //std::printf("HUD: Speed=%03d km/h\n", new_speed);
    }
}

void hook_hud_pos(Cpu&c) {
    uint16_t new_pos =c.bus->read_word(kHudPosAddr);
    if (new_pos != Hud::hud_pos) {
        Hud::hud_pos = new_pos;
        // pos is zero indexed so 0 == 1st, ... 39 == 40th
        //std::printf("HUD: Pos=%02d/40\n", new_pos + 1);
    }
}

void hook_hud_gear(Cpu&c) {
    uint32_t new_gear =c.bus->read_dword(kHudGearAddr);
    if (new_gear != Hud::hud_gear) {
        Hud::hud_gear = new_gear;
        // Gear is zero indexed so 0..3 -> 1-4
        //std::printf("HUD: Gear=%d\n", new_gear + 1);
    }
}

void hook_hud_rpm(Cpu&c) {
    uint32_t new_rpm =c.bus->read_dword(kHudRevsAddr);
    if (new_rpm != Hud::hud_rpm) {
        Hud::hud_rpm = new_rpm;
        //std::printf("HUD: Revs=%04d rpm\n", new_rpm);
    }
}

void hook_hud_lap_cur(Cpu&c) {
    uint32_t new_lap =c.bus->read_dword(kHudLapCurAddr);
    if (new_lap != Hud::hud_lap_cur) {
        Hud::hud_lap_cur = new_lap;
        //std::printf("HUD: Lap=%d/%d\n", new_lap + 1, Hud::hud_lap_tot);
    }
}

void hook_hud_lap_tot(Cpu&c) {
    uint32_t new_lap =c.bus->read_dword(kHudLapTotAddr);
    if (new_lap != Hud::hud_lap_tot) {
        Hud::hud_lap_tot = new_lap;
        //std::printf("HUD: Total Laps=%d\n", new_lap);
    }
}
void hook_hud_gear_prompt_on(Cpu&c) {
    // 'PUT THE STICK TO 4' - shown
    //std::printf("'PUT THE STICK TO 4' SHOWN\n");
    if (Hud::hide_hud_gear)
        c.m_IP += 4;
}

void hook_hud_gear_prompt_off(Cpu&c) {
    // 'PUT THE STICK TO 4' - hidden
    //std::printf("'PUT THE STICK TO 4' HIDDEN\n");
    if (Hud::hide_hud_gear)
        c.m_IP += 4;
}

void hook_skip_call(Cpu &c) {
     c.m_IP += 4;
}

void hook_hide_hud_time(Cpu &c) {
    if (Hud::hide_hud_time)
        c.m_IP += 0x104;
}

void hook_hide_hud_speed(Cpu &c) {
    if (Hud::hide_hud_speed)
        c.m_IP += 4;
}

void hook_hide_hud_gear(Cpu &c) {
    if (Hud::hide_hud_gear)
        c.m_IP += 4;
}

void hook_hide_hud_rpm(Cpu &c) {
    if (Hud::hide_hud_rpm)
        c.m_IP += 4;
}

void hook_hide_hud_laps_init(Cpu &c) {
    if (Hud::hide_hud_laps) {
        if (kIsRevisionA) {
            c.m_IP += 0x8;
        } else {
            c.m_IP += 0x54;
        }
    }
}

void hook_hide_hud_laps_update(Cpu &c) {
    if (Hud::hide_hud_laps) {
        c.m_IP += 4;
    }
}

void hook_hide_hud_pos_init(Cpu &c) {
    if (Hud::hide_hud_pos) {
        c.m_IP += 0x40;
    }
}

void hook_hide_hud_pos_update(Cpu &c) {
    if (Hud::hide_hud_pos) {
        c.m_IP += 4;
    }
}

} // namespace rt
