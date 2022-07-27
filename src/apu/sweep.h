#ifndef SRC_APU_SWEEP_H_
#define SRC_APU_SWEEP_H_

#include <cstdint>

#include "src/apu/pulse_channel.h"

namespace audio {

class Sweep {
 public:
  Sweep(PulseChannel pulse_channel);
  void Clock();
  void Write(uint8_t value);
  bool Muting();

  uint16_t pulse_period = 0x0000;

 private:
  bool Muting(uint16_t target_period);
  uint16_t TargetPeriod();

  bool enabled = false;
  bool negate = false;
  bool restart = true;
  uint8_t shift = 0x00;
  uint8_t period = 0x00;
  uint8_t divider = 0x00;
  PulseChannel channel;
};

}  // namespace audio

#endif  // SRC_APU_SWEEP_H_