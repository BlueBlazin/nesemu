#ifndef SRC_PPU_PPU_H_
#define SRC_PPU_PPU_H_

#include <cstdint>
#include <iostream>

constexpr uint64_t STEP1_CYCLES = 3728 * 2 + 1;
constexpr uint64_t STEP2_CYCLES = 7456 * 2 + 1;
constexpr uint64_t STEP3_CYCLES = 11185 * 2 + 1;
constexpr uint64_t STEP4_CYCLES = 14914 * 2 + 1;
constexpr uint64_t STEP5_CYCLES = 18640 * 2 + 1;
constexpr uint64_t MODE0_RESET = 14915 * 2;
constexpr uint64_t MODE1_RESET = 18641 * 2;

namespace audio {

class Apu {
 public:
  Apu();

  void Tick(uint64_t cycles);

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);

  bool frame_interrupt = false;

 private:
  void ModeZeroTick();
  void ModeOneTick();

  /*---------------------------------------------------
    APU Internal
  ---------------------------------------------------*/
  bool mode0 = true;
  uint64_t half_cycles = 0;
  bool interrupt_inhibit = false;
};

}  // namespace audio

#endif  // SRC_PPU_PPU_H_