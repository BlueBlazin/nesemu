#ifndef SRC_APU_APU_H_
#define SRC_APU_APU_H_

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "src/apu/dmc.h"
#include "src/apu/mixer.h"
#include "src/apu/noise.h"
#include "src/apu/pulse.h"
#include "src/apu/triangle.h"
#include "src/mappers/mapper.h"

namespace audio {

constexpr uint64_t STEP1 = 3728 * 2 + 1;
constexpr uint64_t STEP2 = 7456 * 2 + 1;
constexpr uint64_t STEP3 = 11185 * 2 + 1;
constexpr uint64_t STEP4_1 = 14914 * 2;
constexpr uint64_t STEP4_2 = STEP4_1 + 1;
constexpr uint64_t STEP5 = 18640 * 2 + 1;
constexpr uint64_t MODE0_RESET = 14915 * 2;
constexpr uint64_t MODE1_RESET = 18641 * 2;

constexpr int AUDIO_BUFFER_SIZE = 1024;
constexpr float CPU_FREQUENCY = 1789773.0F;
constexpr float SAMPLE_RATE = 44100.0F;
constexpr float SAMPLE_CLOCKS = CPU_FREQUENCY / SAMPLE_RATE;

class Apu {
 public:
  Apu(std::shared_ptr<mappers::Mapper> mapper);

  void Tick(uint64_t cycles);

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);
  bool AudioBufferFull();
  std::vector<int16_t> GetAudioBuffer();

  bool StallCpu() { return dmc.stall_cpu; }
  bool IrQPending() { return frame_interrupt || dmc.dmc_interrupt; }
  void ClearInterrupts() {
    frame_interrupt = false;
    dmc.dmc_interrupt = false;
  }

  bool frame_interrupt = false;

 private:
  void ClockSequencer();
  void ModeZeroTick();
  void ModeOneTick();
  void ClockEnvelopesAndLinear();
  void ClockLengthAndSweep();

  void Sample();

  std::vector<int16_t> audio_buffer;
  Pulse pulse1;
  Pulse pulse2;
  Triangle triangle;
  Noise noise;
  Dmc dmc;

  /*---------------------------------------------------
    Status
  ---------------------------------------------------*/
  bool dmc_enabled = false;
  bool noise_enabled = false;
  bool triangle_enabled = false;
  bool pulse2_enabled = false;
  bool pulse1_enabled = false;
  /*---------------------------------------------------
    APU Internal
  ---------------------------------------------------*/
  bool mode0 = true;
  uint64_t half_cycles = 0;
  bool interrupt_inhibit = false;
  float sample_counter = 0.0F;
  int frame_reset_delay = 0;
};

}  // namespace audio

#endif  // SRC_APU_APU_H_