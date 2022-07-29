#ifndef SRC_APU_PULSE_H_
#define SRC_APU_PULSE_H_

#include <cstdint>

#include "src/apu/envelope.h"
#include "src/apu/length_counter.h"
#include "src/apu/pulse_channel.h"
#include "src/apu/sweep.h"

namespace audio {

constexpr uint16_t PULSE_DUTY[4][8] = {
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {1, 0, 0, 1, 1, 1, 1, 1},
};

class Pulse {
 public:
  Pulse(PulseChannel pulse_channel);
  void Clock();
  uint16_t Volume();
  void Write(uint16_t addr, uint8_t value);

  Envelope envelope;
  Sweep sweep;
  LengthCounter length_counter;

 private:
  bool clock_toggle = false;
  uint8_t duty_cycle = 0;
  uint8_t phase = 0;
  uint16_t timer = 0x0000;
};

}  // namespace audio

#endif  // SRC_APU_PULSE_H_