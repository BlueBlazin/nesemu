#include "memory.h"

#include <cstdint>

namespace memory {

uint8_t Memory::Read(uint16_t addr) {
  if (addr <= 0x1FFF) {
    return ram[addr % 0x800];
  } else if (addr <= 0x3FFF) {
    // delegate to GPU
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
    // delegate to GPU
  } else if (addr <= 0x4017) {
    // TODO
  } else if (addr <= 0x401F) {
    // TODO
  } else {
    // TODO
  }
}

}  // namespace memory
