#ifndef SRC_APU_APU_H_
#define SRC_APU_APU_H_

#include <cstdint>
#include <iostream>
#include <vector>

#include "src/apu/pulse.h"

namespace audio {

constexpr uint64_t STEP1 = 3728 * 2 + 1;
constexpr uint64_t STEP2 = 7456 * 2 + 1;
constexpr uint64_t STEP3 = 11185 * 2 + 1;
constexpr uint64_t STEP4_1 = 14914 * 2;
constexpr uint64_t STEP4_2 = STEP4_1 + 1;
constexpr uint64_t STEP5 = 18640 * 2 + 1;
constexpr uint64_t MODE0_RESET = 14915 * 2;
constexpr uint64_t MODE1_RESET = 18641 * 2;

constexpr int AUDIO_BUFFER_SIZE = 2048;
constexpr int SAMPLE_RATE = 44100;
constexpr int BUFFER_FILL_TIME = AUDIO_BUFFER_SIZE / SAMPLE_RATE;
constexpr int CPU_FREQUENCY = 1789773;
constexpr int SAMPLE_CLOCKS = CPU_FREQUENCY * BUFFER_FILL_TIME;

constexpr double MIXER_PULSE_TABLE[31] = {0,
                                          0.011609139523578026,
                                          0.022939481268011527,
                                          0.03400094921689606,
                                          0.04480300187617261,
                                          0.05535465924895688,
                                          0.06566452795600367,
                                          0.07574082464884459,
                                          0.08559139784946236,
                                          0.09522374833850243,
                                          0.10464504820333041,
                                          0.11386215864759427,
                                          0.12288164665523155,
                                          0.13170980059397538,
                                          0.14035264483627205,
                                          0.1488159534690486,
                                          0.15710526315789472,
                                          0.16522588522588522,
                                          0.1731829170024174,
                                          0.18098125249301955,
                                          0.18862559241706162,
                                          0.19612045365662886,
                                          0.20347017815646784,
                                          0.21067894131185272,
                                          0.21775075987841944,
                                          0.2246894994354535,
                                          0.2314988814317673,
                                          0.23818248984115256,
                                          0.2447437774524158,
                                          0.2511860718171926,
                                          0.25751258087706685};

class Apu {
 public:
  Apu();

  void Tick(uint64_t cycles);

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);
  bool AudioBufferFull();
  std::vector<int16_t> GetAudioBuffer();

  bool frame_interrupt = false;
  bool dmc_interrupt = false;

 private:
  void ModeZeroTick();
  void ModeOneTick();
  void ClockEnvelopesAndLinear();
  void ClockLengthAndSweep();

  void Sample();

  std::vector<int16_t> audio_buffer;
  Pulse pulse1;
  Pulse pulse2;

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
  int sample_counter = 0;
};

}  // namespace audio

#endif  // SRC_APU_APU_H_