#include "memory.h"

#include <cstdint>
#include <cstdio>
#include <memory>

#include "src/mappers/ines.h"
#include "src/mappers/mapper.h"

namespace memory {

Memory::Memory(const std::string& path, uint8_t& p1_input)
    : apu(),
      cartridge(mappers::ReadCartridge(path)),
      ppu(cartridge),
      ram(),
      p1_input(p1_input) {
  for (int i = 0; i < ram.size(); i++) {
    ram[i] = 0x00;
  }
}

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
      if ((dma_addr & 0xFF) == 0) {
        in_dma = false;
        ppu.UpdateSprites();
      }
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

uint8_t* Memory::GetNametable(uint16_t addr) {
  ppu.UpdateNametable(addr);

  switch (addr) {
    case 0x2000:
      return ppu.nametable1.data();
    case 0x2400:
      return ppu.nametable2.data();
    case 0x2800:
      return ppu.nametable3.data();
    case 0x2C00:
      return ppu.nametable4.data();
  }
}

uint8_t* Memory::GetSprites() {
  // ppu.UpdateSprites();
  return ppu.sprites.data();
}

uint8_t* Memory::GetPalettes() {
  ppu.UpdatePalettes();
  return ppu.palettes.data();
}

bool Memory::NmiPending() { return ppu.NmiOccured(); }

void Memory::ClearNmi() { ppu.ClearNmi(); }

bool Memory::InDma() { return in_dma; }

bool Memory::VblankEvent() { return ppu.vblank_event; }

void Memory::ClearVBlankEvent() { ppu.vblank_event = false; }

uint8_t Memory::Read(uint16_t addr) {
  if (addr <= 0x1FFF) {
    return ram[addr % 0x800];
  } else if (addr <= 0x2007) {
    return ppu.Read(addr);
  } else if (addr <= 0x3FFF) {
    return ppu.Read(0x2000 | ((addr - 0x2000) & 0x7));
  } else if (addr <= 0x4017) {
    switch (addr) {
      case 0x4016: {
        if (strobe) {
          p1_data = p1_input;
        }
        uint8_t value = 0x40 | (p1_data & 0x1);
        p1_data = 0x80 | (p1_data >> 1);
        return value;
      }
      default:
        return 0x00;
    }
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
    ppu.Write(addr, value);
  } else if (addr <= 0x3FFF) {
    return ppu.Write(0x2000 | ((addr - 0x2000) & 0x7), value);
  } else if (addr <= 0x4017) {
    switch (addr) {
      case 0x4014: {
        in_dma = true;
        dma_state = DmaState::Read;
        dma_addr = static_cast<uint16_t>(value) << 8;
        return;
      }
      case 0x4016:
        strobe = static_cast<bool>(value & 0x1);
        if (strobe) {
          p1_data = p1_input;
          // p1_input = 0;
        }
        return;
    }
  } else if (addr <= 0x401F) {
    // TODO
  } else if (addr <= 0xFFFF) {
    return cartridge->CpuWrite(addr, value);
  }
}

}  // namespace memory
