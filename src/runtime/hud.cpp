#include "runtime/hud.h"
#include "runtime/game_revision.h"

#include <cstdlib>

namespace rt {

namespace {
constexpr uint32_t kHudTimeAddr                 =  0x5010d0; /* RevA & 93 */
constexpr uint32_t kHudSpeedAddr                = kIsRevisionA ? 0x505198 /* RevA */ : 0x504e18 /* 93 */;
constexpr uint32_t kHudPosAddr                  = kIsRevisionA ? 0x5051ec /* RevA */ : 0x504e6a /* 93 */;
constexpr uint32_t kHudGearAddr                 = kIsRevisionA ? 0x5052cc /* RevA */ : 0x504f48 /* 93 */;
constexpr uint32_t kHudRevsAddr                 = kIsRevisionA ? 0x5052d8 /* RevA */ : 0x504f54 /* 93 */;
constexpr uint32_t kHudLapCurAddr               =  0x5010e0; /* RevA & 93 */
constexpr uint32_t kHudLapTotAddr               =  0x5010ec; /* RevA & 93 */
constexpr uint32_t kHudTrackIdAddr              =  0x501460; // RevA. Unconfirmed for 93
constexpr uint32_t kHudRollingStartStateAddr    =  0x501850; // RevA. Unconfirmed for 93
constexpr uint32_t kGameStateAddr               =  0x5010a0; // RevA. Unconfirmed for 93
constexpr uint32_t kGameStateFrameCountAddr     =  0x5010a8; // RevA. Unconfirmed for 93
}

// Game state. Likely a bitfield. 0x005010a4 (RevA)
static inline uint32_t game_state = 0;
// Game state frames (countdown to zero) 0x005010a8 (RevA)
static inline uint32_t game_state_frame_cnt = 0;


constexpr const char* trackid_to_string(TrackId track) noexcept {
    switch (track) {
        case TrackId::Beginner: return "Beginner";
        case TrackId::Expert:   return "Expert";
        case TrackId::Advanced: return "Advanced";
    }
    return "Unknown";
}

constexpr const char* start_seq_state_to_string(StartSeq seq) noexcept {
    switch (seq) {
        case StartSeq::Ready: return "Ready";
        case StartSeq::Three: return "3";
        case StartSeq::Two: return "2";
        case StartSeq::One: return "1";
        case StartSeq::Go: return "GO";
        case StartSeq::Race: return "Clear";
        case StartSeq::None: return "None";
    }
    return "Unknown";
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
    //std::printf("HUD: 'PUT THE STICK TO 4' SHOWN\n");
    if (Hud::hide_hud_gear)
        c.m_IP += 4;
}

void hook_hud_gear_prompt_off(Cpu&c) {
    // 'PUT THE STICK TO 4' - hidden
    //std::printf("HUD: 'PUT THE STICK TO 4' HIDDEN\n");
    if (Hud::hide_hud_gear)
        c.m_IP += 4;
}

void hook_hud_track(Cpu&c) {
    uint32_t new_track_id = c.bus->read_dword(kHudTrackIdAddr);
    if (new_track_id != static_cast<uint32_t>(Hud::hud_track_id) &&
        new_track_id >= static_cast<uint32_t>(TrackId::Beginner) &&
        new_track_id <= static_cast<uint32_t>(TrackId::Advanced)) {
        Hud::hud_track_id = static_cast<TrackId>(new_track_id);
        std::printf("HUD: Track=%s\n",  trackid_to_string(Hud::hud_track_id));
    } 
}

void on_start_sequence_state(StartSeq seq) {
    if (seq != Hud::hud_start_seq) {
        Hud::hud_start_seq = seq;
        std::printf("HUD: Start Sequence: %s\n", start_seq_state_to_string(seq));
    }
}

void hook_hud_roll_start(Cpu&c) {
    uint32_t seq = c.bus->read_dword(kHudRollingStartStateAddr);
    switch (seq) {
    case 0x00:  // Ready
    case 0x01:  // Three
    case 0x02:  // Two
    case 0x03:  // One
    case 0x05:  // GO
    case 0x40:  // RACE
        on_start_sequence_state(static_cast<StartSeq>(seq));
        break;
    default:
        break;
    }
}

void hook_game_state(Cpu&c) {
    game_state = c.bus->read_dword(kGameStateAddr);
}

void hook_game_state_frame_cnt(Cpu&c) {
    game_state_frame_cnt = c.bus->read_dword(kGameStateFrameCountAddr);
    if (0x14 == game_state && Hud::hud_track_id != TrackId::Beginner) {
        // 0x14 == standing start sequence (Advanced & Expert tracks only)
        // Probably a better way to do this but I haven't found it yet!
        switch (game_state_frame_cnt) {
        case 0x13f: // Ready
            on_start_sequence_state(StartSeq::Ready);
            break;
        case 0xbf:  // 3
            on_start_sequence_state(StartSeq::Three);
            break;
        case 0x8e:  // 2
            on_start_sequence_state(StartSeq::Two);
            break;
        case 0x4e:  // 1
            on_start_sequence_state(StartSeq::One);
            break;
        case 0x0e:  // GO
            on_start_sequence_state(StartSeq::Go);
            break;
        case 0x00:  // RACE
            on_start_sequence_state(StartSeq::Race);
            break;
        default:
            break;
        }
    }
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
