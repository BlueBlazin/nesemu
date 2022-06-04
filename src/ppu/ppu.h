#ifndef SRC_PPU_PPU_
#define SRC_PPU_PPU_

#include <array>
#include <cstdint>

namespace graphics {

class Ppu {
 public:
  Ppu();

  void Tick(uint64_t ppu_cycles);

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);

 private:
  uint64_t line = 0;
  uint64_t clock = 0;
  uint64_t frame = 0;
  uint64_t cycles = 0;

  uint8_t ppu_ctrl = 0x0;
  uint8_t ppu_mask = 0x0;
  uint8_t ppu_status = 0x0;
  uint8_t oam_addr = 0x0;
  uint8_t oam_data = 0x0;
  uint8_t ppu_scroll = 0x0;
  uint8_t ppu_addr = 0x0;
  uint8_t ppu_data = 0x0;
  uint8_t oam_dma = 0x0;
};

}  // namespace graphics

#endif  // SRC_PPU_PPU_