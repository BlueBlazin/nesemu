#include "sweep.h"

#include "src/apu/pulse_channel.h"

namespace audio {

Sweep::Sweep(PulseChannel pulse_channel) : channel(pulse_channel) {}

bool Sweep::Muting() { return Muting(TargetPeriod()); }

void Sweep::Clock() {
  uint16_t target_period = TargetPeriod();

  if (divider == 0 && enabled && !Muting(target_period)) {
    pulse_period = target_period;
  } else if (divider == 0 || restart) {
    divider = period;
    restart = false;
  } else {
    divider--;
  }
}

uint16_t Sweep::TargetPeriod() {
  int16_t change = static_cast<int16_t>(pulse_period >> shift);

  if (negate) {
    switch (channel) {
      case PulseChannel::Pulse1: {
        if (shift == 0 || pulse_period == 0) {
          return 0x0000;
        }
        change = -change - 1;
        break;
      }
      case PulseChannel::Pulse2:
        change = -change;
        break;
    }
  }

  return static_cast<uint16_t>(static_cast<int16_t>(pulse_period) + change);
}

bool Sweep::Muting(uint16_t target_period) {
  return pulse_period < 8 || target_period > 0x7FF;
}

void Sweep::Write(uint8_t value) {
  enabled = static_cast<bool>(value & 0x80);
  period = (value >> 4) & 0x7;
  negate = static_cast<bool>(value & 0x8);
  shift = value & 0x7;
  restart = true;
}

}  // namespace audio
