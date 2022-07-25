#include "envelope.h"

namespace audio {

Envelope::Envelope() {}

void Envelope::Write(uint8_t value) {
  loop = static_cast<bool>(value & 0x20);
  const_volume = static_cast<bool>(value & 0x10);
  volume = value & 0xF;
  period = volume + 1;
}

}  // namespace audio
