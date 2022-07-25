#ifndef SRC_APU_PULSE_H_
#define SRC_APU_PULSE_H_

#include <cstdint>

namespace audio {

constexpr uint8_t PULSE_DUTY[4][8] = {
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {1, 0, 0, 1, 1, 1, 1, 1},
};

class Pulse {
 public:
  Pulse();
  void Clock();
  void Write(uint16_t addr, uint8_t value);

  Envelope envelope;

 private:
  uint8_t duty_cycle = 0;
  uint8_t waveform_idx = 0;
  uint16_t period = 0x0000;
  uint16_t timer = 0x0000;
};

}  // namespace audio

#endif  // SRC_APU_PULSE_H_