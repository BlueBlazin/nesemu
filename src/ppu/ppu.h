#ifndef SRC_PPU_PPU_
#define SRC_PPU_PPU_

#include <array>
#include <cstdint>

namespace graphics {

class Ppu {
 public:
  Ppu();

  void Tick(uint64_t cycles);

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);

 private:
  uint64_t line = 0;
  uint64_t clock = 0;
  uint64_t frame = 0;

  // 0x2000 - 0x2007 and repeated till 0x3FFF
  std::array<uint8_t, 8> regs = {0, 0, 0, 0, 0, 0, 0, 0};
  // 0x4014
  uint8_t oam_dma = 0;
};

}  // namespace graphics

#endif  // SRC_PPU_PPU_