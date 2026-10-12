// Enhancements that act inside the recompiled game code, through hooks the
// recompiler places (m2recomp --hooks, seeds/daytona93_hooks.txt): just before
// the instruction at a hook's address, the generated code calls
// rt::hook_<name>(c). All off by default; while off a hook returns at once,
// so the game runs exactly as recompiled (rules.md: parity runs that way).
#pragma once

#include "runtime/cpu.h"

namespace rt {

struct Hud {
    static inline uint32_t hud_time     = 0;
    static inline uint32_t hud_speed    = 0;
    static inline uint16_t hud_pos      = 0;
    static inline uint32_t hud_gear     = 0;
    static inline uint32_t hud_rpm      = 0;
    static inline uint32_t hud_lap_cur  = 0;
    static inline uint32_t hud_lap_tot  = 0;
};

void hook_hud_time(Cpu &c);
void hook_hud_speed(Cpu&c);
void hook_hud_pos(Cpu&c);
void hook_hud_gear(Cpu&c);
void hook_hud_rpm(Cpu&c);
void hook_hud_lap_cur(Cpu&c);
void hook_hud_lap_tot(Cpu&c);

} // namespace rt
