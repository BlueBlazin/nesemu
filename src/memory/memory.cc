#include "memory.h"

#include <cstdint>
#include <memory>

#include "src/mappers/ines.h"
#include "src/mappers/mapper.h"

namespace memory {

Memory::Memory(const std::string& path)
    : cartridge(mappers::ReadCartridge(path)), ppu(cartridge) {
  // cartridge = mappers::ReadCartridge(path);
  // ppu = graphics::Ppu(cartridge);
}

uint8_t Memory::Read(uint16_t addr) {
  if (addr <= 0x1FFF) {
    return ram[addr % 0x800];
  } else if (addr <= 0x3FFF) {
    return ppu.Read(addr);
  } else if (addr <= 0x4017) {
    // TODO
  } else if (addr <= 0x401F) {
    // TODO
  } else {
    // TODO
  }
}

void Memory::Write(uint16_t addr, uint8_t value) {
  if (addr <= 0x1FFF) {
    ram[addr % 0x800] = value;
  } else if (addr <= 0x3FFF) {
    ppu.Write(addr, value);
  } else if (addr <= 0x4017) {
    // TODO
  } else if (addr <= 0x401F) {
    // TODO
  } else {
    // TODO
  }
}

}  // namespace memory
