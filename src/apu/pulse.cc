#include "pulse.h"

#include "src/apu/envelope.h"

namespace audio {

Pulse::Pulse() : envelope() {}

void Pulse::Clock() {
  timer--;

  if (timer == 0) {
    // reset timer
    timer = period;

    if (waveform_idx == 0) {
      waveform_idx = 7;
    } else {
      waveform_idx--;
    }
  }
}

void Pulse::Write(uint16_t addr, uint8_t value) {
  switch (addr) {
    case 0x4000:
    case 0x4004:
      duty_cycle = (value >> 6);
      envelope.Write(value);
      break;
  }
}

}  // namespace audio
