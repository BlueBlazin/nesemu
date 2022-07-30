#include "length_counter.h"

namespace audio {

LengthCounter::LengthCounter() {}

void LengthCounter::Clock() {
  if (length > 0 && !halt) {
    length--;
  } else {
    length = 0;
  }
}

void LengthCounter::Write(uint8_t value) {
  if (enabled) {
    entry = (value >> 3);
    length = LENGTHS[entry];
  } else {
    length = 0;
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
