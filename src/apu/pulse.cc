#include "pulse.h"

#include "src/apu/envelope.h"

namespace audio {

Pulse::Pulse(PulseChannel pulse_channel)
    : envelope(), sweep(pulse_channel), length_counter() {}

void Pulse::Clock() {
  timer--;

  if (timer == 0) {
    // reset timer
    timer = sweep.pulse_period;

    if (waveform_idx == 0) {
      waveform_idx = 7;
    } else {
      waveform_idx--;
    }
  }
}

uint16_t Pulse::Volume() {
  if (sweep.Muting() || length_counter.Muting()) {
    return 0x0000;
  }

  uint16_t value = PULSE_DUTY[duty_cycle][waveform_idx];
  return value * static_cast<uint16_t>(envelope.Volume());
}

void Pulse::Write(uint16_t addr, uint8_t value) {
  switch (addr) {
    case 0x4000:
    case 0x4004:
      duty_cycle = (value >> 6);
      envelope.Write(value);
      break;
    case 0x4001:
    case 0x4005:
      sweep.Write(value);
      break;
    case 0x4002:
    case 0x4006:
      period_low = static_cast<uint16_t>(value);
      break;
    case 0x4003:
    case 0x4007:
      sweep.pulse_period =
          (static_cast<uint16_t>(value & 0x7) << 8) | period_low;
      // TODO: length counter load
      timer = sweep.pulse_period;
      waveform_idx = 0;
      break;
  }
}

}  // namespace audio
