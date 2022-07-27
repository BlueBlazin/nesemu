#ifndef SRC_APU_LENGTH_COUNTER_H_
#define SRC_APU_LENGTH_COUNTER_H_

#include <cstdint>

namespace audio {

constexpr uint8_t LENGTHS[32] = {10, 254, 20,  2,  40, 4,  80, 6,  160, 8,  60,
                                 10, 14,  12,  26, 14, 12, 16, 24, 18,  48, 20,
                                 96, 22,  192, 24, 72, 26, 16, 28, 32,  30};

class LengthCounter {
 public:
  LengthCounter();
  void Clock();
  void Write(uint8_t value);
  void WriteHalt(bool value);
  void SetEnabled(bool value);

  bool Muting() { return enabled && length == 0; }

 private:
  bool enabled = false;
  bool halt = false;
  uint8_t entry = 0x00;
  uint8_t length = 0x00;
};

}  // namespace audio

#endif  // SRC_APU_LENGTH_COUNTER_H_