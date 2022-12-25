#include "uxrom.h"

namespace mappers {

UxRom::UxRom(INesHeader header, std::vector<uint8_t> data)
    : prg_rom(), chr_rxm(), vram(), mirroring(header.mirroring) {
  if (header.prg_rom_size % BANK_SIZE != 0) {
    throw "INES ROM size not a multiple of 16K";
  }

  num_banks = header.prg_rom_size / BANK_SIZE;

  uint64_t offset = INES_HEADER_SIZE + (header.has_trainer ? TRAINER_SIZE : 0);

  prg_rom = std::vector<uint8_t>(data.begin() + offset,
                                 data.begin() + offset + header.prg_rom_size);

  offset += header.prg_rom_size;

  chr_rxm = std::vector<uint8_t>(data.begin() + offset,
                                 data.begin() + offset + header.chr_rom_size);

  std::cout << "Mapper type: UxROM" << std::endl;
  std::cout << "num_banks: " << num_banks << std::endl;
  std::cout << "prg_rom: " << prg_rom.size() << std::endl;
  std::cout << "chr_rxm: " << chr_rxm.size() << std::endl;
  std::cout << "vram: " << vram.size() << std::endl;
}

uint8_t UxRom::CpuRead(uint16_t addr) {
  if (addr < 0x8000) {
    return 0x00;
  } else if (addr <= 0xBFFF) {
    return prg_rom[bank * BANK_SIZE + (addr - 0x8000)];
  } else if (addr <= 0xFFFF) {
    return prg_rom[(num_banks - 1) * BANK_SIZE + (addr - 0xC000)];
  } else {
    return 0x00;
  }
}

void UxRom::CpuWrite(uint16_t addr, uint8_t value) {
  if (addr >= 0x8000) {
    bank = static_cast<uint16_t>(value & 0xF);
  }
}

uint8_t UxRom::PpuRead(uint16_t addr) {
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

void UxRom::PpuWrite(uint16_t addr, uint8_t value) {
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

uint8_t UxRom::VramRead(uint16_t addr) {
  switch (mirroring) {
    case graphics::Mirroring::Horizontal: {
      return vram[graphics::horizontal_mirrored(addr)];
    }
    case graphics::Mirroring::Vertical: {
      return vram[graphics::vertical_mirrored(addr)];
    }
  }
}

void UxRom::VramWrite(uint16_t addr, uint8_t value) {
  switch (mirroring) {
    case graphics::Mirroring::Horizontal: {
      vram[graphics::horizontal_mirrored(addr)] = value;
      return;
    }
    case graphics::Mirroring::Vertical: {
      vram[graphics::vertical_mirrored(addr)] = value;
      return;
    }
  }
}

}  // namespace mappers
