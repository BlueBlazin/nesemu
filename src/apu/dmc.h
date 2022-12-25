#ifndef SRC_APU_DMC_H_
#define SRC_APU_DMC_H_

#include <cstdint>
#include <memory>

#include "src/mappers/mapper.h"

namespace audio {

constexpr uint16_t RATES[16] = {428, 380, 340, 320, 286, 254, 226, 214,
                                190, 160, 142, 128, 106, 84,  72,  54};

class Dmc {
 public:
  Dmc(std::shared_ptr<mappers::Mapper> mapper);
  void Clock();
  uint16_t Volume();
  void Write(uint16_t addr, uint8_t value);
  void SetEnabled(bool value);

  bool dmc_interrupt = false;
  bool stall_cpu = false;
  bool silence = false;
  uint16_t bytes_remaining = 0x0000;

 private:
  void ClockOutputCycle();
  void StartNewOutputCycle();
  void NextSampleByte();
  void RestartSample();

  bool irq_enable = false;
  bool loop = false;
  uint16_t rate = 0x0000;
  uint16_t timer = 0x0000;
  uint16_t output_level = 0x00000;
  uint16_t sample_addr_raw = 0x0000;
  uint16_t sample_length_raw = 0x0000;
  uint16_t sample_addr = 0x0000;
  uint8_t sample_buffer = 0x00;
  bool sample_buffer_emptied = true;
  uint8_t shift_register = 0x00;
  uint8_t bits_remaining = 0x00;
  bool restart_pending = false;

  std::shared_ptr<mappers::Mapper> cartridge;
};

}  // namespace audio

#endif  // SRC_APU_DMC_H_
