#include "length_counter.h"

namespace audio {

LengthCounter::LengthCounter() {}

void LengthCounter::Clock() {
  if (length > 0 && !halt) {
    length--;
  }
}

void LengthCounter::Write(uint8_t value) {
  entry = (value >> 3) & 0x1F;
  if (enabled) {
    length = LENGTHS[entry];
  }
}

void LengthCounter::WriteHalt(bool value) { halt = value; }

void LengthCounter::SetEnabled(bool value) {
  enabled = value;
  if (!enabled) {
    length = 0;
  }
}

}  // namespace audio
