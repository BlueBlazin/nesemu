#include "pulse.h"

#include <iostream>

#include "src/apu/envelope.h"

namespace audio {

Pulse::Pulse(PulseChannel pulse_channel)
    : envelope(), sweep(pulse_channel), length_counter() {}

void Pulse::Clock() {
  if (clock_toggle) {
    timer--;

    if (timer == 0) {
      // reset timer
      timer = sweep.pulse_period;

      if (phase == 0) {
        phase = 7;
      } else {
        phase--;
      }
    }
  }

  clock_toggle = !clock_toggle;
}

uint16_t Pulse::Volume() {
  if (sweep.Muting() || length_counter.Muting()) {
    return 0x0000;
  }

  uint16_t value = PULSE_DUTY[duty_cycle][phase];
  return value * static_cast<uint16_t>(envelope.Volume());
}

void Pulse::Write(uint16_t addr, uint8_t value) {
  switch (addr) {
    case 0x4000:
    case 0x4004:
      duty_cycle = (value >> 6);
      envelope.Write(value);
      length_counter.WriteHalt(static_cast<bool>(value & 0x20));
      break;
    case 0x4001:
    case 0x4005:
      sweep.Write(value);
      break;
    case 0x4002:
    case 0x4006:
      sweep.pulse_period =
          (sweep.pulse_period & 0xFF00) | static_cast<uint16_t>(value);
      break;
    case 0x4003:
    case 0x4007:
      sweep.pulse_period = (static_cast<uint16_t>(value & 0x7) << 8) |
                           (sweep.pulse_period & 0x00FF);
      length_counter.Write(value);
      envelope.Restart();
      phase = 0;
      break;
  }
}

}  // namespace audio
