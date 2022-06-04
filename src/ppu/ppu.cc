#include "ppu.h"

#include <cstdint>

namespace graphics {

void Ppu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    //
  }
}

uint8_t Ppu::Read(uint16_t addr) {
  if (addr <= 0x3FFF) {
    return regs[(addr - 0x2000) % 8];
  }
}

void Ppu::Write(uint16_t addr, uint8_t value) {
  if (addr <= 0x3FFF) {
    regs[(addr - 0x2000) % 8] = value;
  }
}

}  // namespace graphics
