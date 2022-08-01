#include "noise.h"

#include <bitset>
#include <iostream>

namespace audio {

Noise::Noise() : length_counter(), envelope() {}

void Noise::Clock() {
  if (timer == 0) {
    uint16_t value =
        mode ? ((shift_register >> 6) & 0x1) : ((shift_register >> 1) & 0x1);
    uint16_t bit0 = shift_register & 0x1;
    uint16_t feedback = bit0 ^ value;
    shift_register = (feedback << 14) | (shift_register >> 1);

    timer = period - 1;
  } else {
    timer--;
  }
}

uint16_t Noise::Volume() {
  if (static_cast<bool>(shift_register & 0x1) || length_counter.Muting()) {
    return 0x0000;
  }

  return static_cast<uint16_t>(envelope.Volume());
}

void Noise::Write(uint16_t addr, uint8_t value) {
  switch (addr) {
    case 0x400C: {
      length_counter.WriteHalt(static_cast<bool>(value & 0x20));
      envelope.Write(value);
      break;
    }
    case 0x400E: {
      mode = static_cast<bool>(value & 0x80);
      period = PERIODS[value & 0xF];
      // timer = period;
      break;
    }
    case 0x400F: {
      length_counter.Write(value);
      envelope.Restart();
      break;
    }
    default:
      break;
  }
}

}  // namespace audio
