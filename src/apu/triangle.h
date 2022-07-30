#ifndef SRC_APU_TRIANGLE_H_
#define SRC_APU_TRIANGLE_H_

#include <cstdint>

#include "src/apu/length_counter.h"

constexpr uint8_t VALUES[32] = {15, 14, 13, 12, 11, 10, 9,  8,  7,  6, 5,
                                4,  3,  2,  1,  0,  0,  1,  2,  3,  4, 5,
                                6,  7,  8,  9,  10, 11, 12, 13, 14, 15};

namespace audio {

class Triangle {
 public:
  Triangle();
  void Clock();
  void ClockLinear();
  uint16_t Volume();
  void Write(uint16_t addr, uint8_t value);

  LengthCounter length_counter;

 private:
  bool control = false;
  uint8_t linear_counter = 0x00;
  uint8_t counter_reload_value = 0x00;
  bool counter_reload_flag = false;
  uint16_t timer = 0x0000;
  uint16_t period = 0x0000;
  int idx = 0;
};

}  // namespace audio

#endif  // SRC_APU_TRIANGLE_H_