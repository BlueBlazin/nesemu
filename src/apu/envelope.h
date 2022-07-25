#ifndef SRC_APU_ENVELOPE_H_
#define SRC_APU_ENVELOPE_H_

#include <cstdint>

namespace audio {

class Envelope {
 public:
  Envelope();

  void Clock();
  void Write(uint8_t value);

  bool start = true;

 private:
  bool loop = false;
  bool const_volume = false;
  uint8_t decay = 15;
  uint16_t period = 0x0000;
  uint8_t volume = 0x00;
};

}  // namespace audio

#endif  // SRC_APU_ENVELOPE_H_