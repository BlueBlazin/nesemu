#include "triangle.h"

namespace audio {

Triangle::Triangle() : length_counter() {}

void Triangle::Clock() {
  if (linear_counter == 0 || length_counter.Muting()) {
    return;
  }

  if (timer == 0) {
    idx = (idx + 1) % 32;
    timer = period;
  } else {
    timer--;
  }
}

void Triangle::ClockLinear() {
  if (counter_reload_flag) {
    linear_counter = counter_reload_value;
  } else {
    if (linear_counter > 0) {
      linear_counter--;
    }
  }

  if (!control) {
    counter_reload_flag = false;
  }
}

uint16_t Triangle::Volume() {
  if (linear_counter == 0 || length_counter.Muting()) {
    return 0x00;
  }

  return VALUES[idx];
}

void Triangle::Write(uint16_t addr, uint8_t value) {
  switch (addr) {
    case 0x4008: {
      control = static_cast<bool>(value & 0x80);
      length_counter.WriteHalt(control);
      counter_reload_value = value & 0x7F;
      break;
    }
    case 0x400A: {
      timer = (timer & 0xFF00) | static_cast<uint16_t>(value);
      period = timer;
      break;
    }
    case 0x400B: {
      timer = (static_cast<uint16_t>(value & 0x7) << 8) | (timer & 0x00FF);
      period = timer;
      length_counter.Write(value);
      counter_reload_flag = true;
      break;
    }
    default:
      break;
  }
}

}  // namespace audio
