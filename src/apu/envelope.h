#ifndef SRC_APU_ENVELOPE_H_
#define SRC_APU_ENVELOPE_H_

#include <cstdint>

namespace audio {

class Envelope {
 public:
  Envelope();
  void Clock();
  void Write(uint8_t value);

  uint8_t Volume() { return const_volume ? volume : decay; }
  void Restart() { restart = true; }

  bool restart = true;

 private:
  bool loop = false;
  bool const_volume = false;
  uint8_t decay = 15;
  uint16_t divider = 0x0000;
  uint16_t period = 0x0000;
  uint8_t volume = 0x00;
};

}  // namespace audio

#endif  // SRC_APU_ENVELOPE_H_