#ifndef SRC_APU_NOISE_H_
#define SRC_APU_NOISE_H_

#include <cstdint>

#include "src/apu/envelope.h"
#include "src/apu/length_counter.h"

constexpr int PERIODS[16] = {4,   8,   16,  32,  64,  96,   128,  160,
                             202, 254, 380, 508, 762, 1016, 2034, 4068};

namespace audio {

class Noise {
 public:
  Noise();
  void Clock();
  uint16_t Volume();
  void Write(uint16_t addr, uint8_t value);

  LengthCounter length_counter;
  Envelope envelope;

 private:
  bool mode = false;
  uint16_t period = 0x00;
  uint16_t timer = 0x00;
  uint16_t shift_register = 0x0001;
};

}  // namespace audio

#endif  // SRC_APU_NOISE_H_