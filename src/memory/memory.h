#ifndef SRC_MEMORY_MEMORY_
#define SRC_MEMORY_MEMORY_

#include <cstdint>

namespace memory {

class Memory {
 public:
  Memory();

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);

 private:
  /* CPU Memory map */
  /* -------------------------------------------- */
  /* 0x0000 - 0x07FF 2KB internal RAM */
  uint8_t ram[0x800];
  /* 0x0800 - 0x0FFF Mirror of 0x0000-0x07FF */
  // virtual

  /* 0x1000 - 0x17FF Mirror of 0x0000-0x07FF */
  // virtual

  /* 0x1800 - 0x1FFF Mirror of 0x0000-0x07FF */
  // virtual

  /* 0x2000 - 0x2007 NES PPU registers */
  // owned by PPU

  /* 0x2008 - 0x3FFF */
  // virtual

  /* 0x4000 - 0x4017 NES APU and I/O registers */
  // TODO: implement these

  /* 0x4018 - 0x401F APU and I/O functionality */
  // TODO: implement these

  /* 0x4020 - 0xFFFF Cartridge space: PRG ROM, PRG RAM, mapper regs */
  // TODO: implement these
};

}  // namespace memory

#endif
