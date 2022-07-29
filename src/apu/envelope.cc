#include "envelope.h"

namespace audio {

Envelope::Envelope() {}

void Envelope::Clock() {
  if (restart) {
    restart = false;
    decay = 15;
    divider = period;
    return;
  }

  if (divider == 0) {
    divider = period;

    if (decay == 0) {
      if (loop) {
        decay = 15;
      }
    } else {
      decay--;
    }
  } else {
    divider--;
  }
}

void Envelope::Write(uint8_t value) {
  loop = static_cast<bool>(value & 0x20);
  const_volume = static_cast<bool>(value & 0x10);
  volume = value & 0xF;
  period = volume;
}

}  // namespace audio
