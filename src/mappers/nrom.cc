#include "nrom.h"

#include <array>
#include <cstdint>
#include <ios>
#include <iostream>
#include <vector>

#include "src/mappers/ines.h"
#include "src/mirroring/mirroring.h"

namespace mappers {

Nrom::Nrom(INesHeader header, std::vector<uint8_t> data)
    : nrom256(header.prg_rom_size == 32 * 1024),
      prg_ram(),
      vram(),
      mirroring(header.mirroring) {
  uint64_t offset = INES_HEADER_SIZE + (header.has_trainer ? TRAINER_SIZE : 0);

  prg_rom = std::vector<uint8_t>(data.begin() + offset,
                                 data.begin() + offset + header.prg_rom_size);

  offset += header.prg_rom_size;

  chr_rxm = std::vector<uint8_t>(data.begin() + offset,
                                 data.begin() + offset + header.chr_rom_size);

  std::cout << "nrom256: " << nrom256 << std::endl;
  std::cout << "prg_rom: " << prg_rom.size() << std::endl;
  std::cout << "prg_ram: " << prg_ram.size() << std::endl;
  std::cout << "chr_rxm: " << chr_rxm.size() << std::endl;
  std::cout << "vram: " << vram.size() << std::endl;
}

uint8_t Nrom::CpuRead(uint16_t addr) {
  if (addr < 0x6000) {
    return 0x00;
  } else if (addr <= 0x7FFF) {
    return prg_ram[addr - 0x6000];
  } else if (addr <= 0xFFFF) {
    return prg_rom[nrom256 ? (addr - 0x8000) : (addr - 0x8000) % 0x4000];
  } else {
    return 0x00;
  }
}

void Nrom::CpuWrite(uint16_t addr, uint8_t value) {
  if (addr < 0x6000) {
    return;
  } else if (addr <= 0x7FFF) {
    prg_ram[addr - 0x6000] = value;
  } else if (addr <= 0xFFFF) {
    prg_rom[nrom256 ? (addr - 0x8000) : (addr - 0x8000) % 0x4000] = value;
  } else {
    return;
  }
}

uint8_t Nrom::PpuRead(uint16_t addr) {
  if (addr <= 0x1FFF) {
    return chr_rxm[addr];
  } else if (addr <= 0x2FFF) {
    return VramRead(addr);
  } else if (addr <= 0x3FFF) {
    return VramRead(addr - 0x1000);
  } else {
    return 0x00;
  }
}

void Nrom::PpuWrite(uint16_t addr, uint8_t value) {
  if (addr <= 0x1FFF) {
    chr_rxm[addr] = value;
  } else if (addr <= 0x2FFF) {
    VramWrite(addr, value);
  } else if (addr <= 0x3FFF) {
    VramWrite(addr - 0x1000, value);
  } else {
    return;
  }
}

uint8_t Nrom::VramRead(uint16_t addr) {
  switch (mirroring) {
    case graphics::Mirroring::Horizontal: {
      if (addr < 0x2400) {
        return vram[addr - 0x2000 + 0x0000];
      } else if (addr < 0x2800) {
        return vram[addr - 0x2400 + 0x0000];
      } else if (addr < 0x2C00) {
        return vram[addr - 0x2800 + 0x0400];
      } else {
        return vram[addr - 0x2C00 + 0x0400];
      }
    }
    case graphics::Mirroring::Vertical: {
      if (addr < 0x2400) {
        return vram[addr - 0x2000 + 0x0000];
      } else if (addr < 0x2800) {
        return vram[addr - 0x2400 + 0x0400];
      } else if (addr < 0x2C00) {
        return vram[addr - 0x2800 + 0x0000];
      } else {
        return vram[addr - 0x2C00 + 0x0400];
      }
    }
  }
}

void Nrom::VramWrite(uint16_t addr, uint8_t value) {
  switch (mirroring) {
    case graphics::Mirroring::Horizontal: {
      if (addr < 0x2400) {
        vram[addr - 0x2000 + 0x0000] = value;
      } else if (addr < 0x2800) {
        vram[addr - 0x2400 + 0x0000] = value;
      } else if (addr < 0x2C00) {
        vram[addr - 0x2800 + 0x0400] = value;
      } else {
        vram[addr - 0x2C00 + 0x0400] = value;
      }
      break;
    }
    case graphics::Mirroring::Vertical: {
      if (addr < 0x2400) {
        vram[addr - 0x2000 + 0x0000] = value;
      } else if (addr < 0x2800) {
        vram[addr - 0x2400 + 0x0400] = value;
      } else if (addr < 0x2C00) {
        vram[addr - 0x2800 + 0x0000] = value;
      } else {
        vram[addr - 0x2C00 + 0x0400] = value;
      }
      break;
    }
  }
}

}  // namespace mappers