#include "memory.h"

#include <cstdint>
#include <cstdio>
#include <memory>

#include "src/mappers/ines.h"
#include "src/mappers/mapper.h"

namespace memory {

Memory::Memory(const std::string& path)
    : cartridge(mappers::ReadCartridge(path)), ppu(cartridge), ram() {
  for (int i = 0; i < ram.size(); i++) {
    ram[i] = 0x00;
  }
}

void Memory::PpuTick(uint64_t n) { ppu.Tick(n); }

void Memory::DmaTick() {
  switch (dma_state) {
    case DmaState::Read: {
      dma_data = Read(dma_addr);
      dma_state = DmaState::Write;
      return;
    }
    case DmaState::Write: {
      ppu.OamDmaWrite(dma_data);
      dma_addr++;
      dma_state = DmaState::Read;
      in_dma = (dma_addr & 0xFF) != 0;
      return;
    }
  }
}

uint8_t* Memory::GetScreen() { return ppu.screen.data(); }

uint8_t* Memory::GetPatTable1() {
  ppu.UpdatePatternTable();
  return ppu.pat_table1.data();
}

uint8_t* Memory::GetPatTable2() {
  ppu.UpdatePatternTable(0x1000);
  return ppu.pat_table2.data();
}

bool Memory::NmiPending() { return ppu.NmiOccured(); }

void Memory::ClearNmi() { ppu.ClearNmi(); }

bool Memory::InDma() { return in_dma; }

uint8_t Memory::Read(uint16_t addr) {
  if (addr <= 0x1FFF) {
    return ram[addr % 0x800];
  } else if (addr <= 0x2007) {
    // std::printf("read addr: 0x%X\n", addr);
    return ppu.Read(addr);
  } else if (addr <= 0x3FFF) {
    return ppu.Read(0x2000 | ((addr - 0x2000) & 0x7));
  } else if (addr <= 0x4017) {
    // TODO
    return 0x00;
  } else if (addr <= 0x401F) {
    // TODO
    return 0x00;
  } else if (addr <= 0xFFFF) {
    return cartridge->CpuRead(addr);
  } else {
    return 0x00;
  }
}

void Memory::Write(uint16_t addr, uint8_t value) {
  if (addr <= 0x1FFF) {
    ram[addr % 0x800] = value;
  } else if (addr <= 0x2007) {
    // std::printf("write addr: 0x%X\n", addr);
    ppu.Write(addr, value);
  } else if (addr <= 0x3FFF) {
    return ppu.Write(0x2000 | ((addr - 0x2000) & 0x7), value);
  } else if (addr <= 0x4017) {
    switch (addr) {
      case 0x4014: {
        in_dma = true;
        dma_state = DmaState::Read;
        dma_addr = static_cast<uint16_t>(value) << 8;
      }
    }
  } else if (addr <= 0x401F) {
    // TODO
  } else if (addr <= 0xFFFF) {
    return cartridge->CpuWrite(addr, value);
  }
}

}  // namespace memory
