#include "ppu.h"

#include <cstdint>

#include "src/ppu/state.h"

namespace graphics {

void Ppu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    PixelTick();
    DataFetcherTick();
    SpriteEvalTick();

    clock++;
    cycles--;
  }
}

void Ppu::DataFetcherTick() {
  switch (scanline_type) {
    case ScanlineType::PreRender:
      return PreRenderTick();
    case ScanlineType::Visible:
      return VisibleTick();
    case ScanlineType::SpriteDataFetch:
      return SpriteDataFetchTick();
    case ScanlineType::TileDataFetch:
      return TileDataFetchTick();
    case ScanlineType::UnkFetch:
      return UnkFetchTick();
    case ScanlineType::PostRender:
      return PostRenderTick();
    case ScanlineType::VBlank:
      return VBlankTick();
  }
}

void Ppu::PreRenderTick() {
  //
}

void Ppu::VisibleTick() {
  switch (cycle_type) {
    case CycleType::Cycle0: {
      cycle_type = CycleType::NametableByte0;
      return;
    }
    case CycleType::NametableByte0: {
      cycle_type = CycleType::NametableByte1;
      return;
    }
    case CycleType::NametableByte1: {
      cycle_type = CycleType::AttrByte0;
      return;
    }
  }
}

void Ppu::SpriteDataFetchTick() {
  //
}

void Ppu::TileDataFetchTick() {
  //
}

void Ppu::UnkFetchTick() {
  //
}

void Ppu::PostRenderTick() {
  //
}

void Ppu::VBlankTick() {
  //
}

uint8_t Ppu::Read(uint16_t addr) {
  switch (addr) {
    case 0x2002:
      return ReadPpuStatus();
    case 0x2004:
      return 0x00;  // TODO
    case 0x2007:
      return ReadPpuData(addr);
    default:
      return last_write;
  }
}

void Ppu::Write(uint16_t addr, uint8_t value) {
  switch (addr) {
    case 0x2000:
      WritePpuCtrl(value);
      break;
    case 0x2001:
      WritePpuMask(value);
      break;
    case 0x2003:
      oam_addr = static_cast<uint16_t>(value);
      break;
    case 0x2004:
      break;  // TODO
    case 0x2005:
      WritePpuScroll(value);
      break;
    case 0x2006:
      WritePpuAddr(value);
      break;
    case 0x2007:
      WritePpuData(value);
      break;
    case 0x4014:
      break;  // TODO
  }

  last_write = value;
}

void Ppu::WritePpuCtrl(uint8_t value) {
  base_nametable_addr = CalcNametableAddr(value & 0x3);
  vram_addr_inc = static_cast<bool>((value >> 2) & 0x1) ? 32 : 1;
  sprite_table_addr = ((value >> 3) & 0x1) ? 0x1000 : 0x0;
  pattern_table_addr = ((value >> 4) & 0x1) ? 0x1000 : 0x0;
  long_sprites = static_cast<bool>((value >> 5) & 0x1);
  ppu_select = static_cast<bool>((value >> 6) & 0x1);
  generate_vblank_nmi = static_cast<bool>((value >> 7) & 0x1);

  reg_T |= static_cast<uint16_t>(value & 0x3) << 10;
}

void Ppu::WritePpuMask(uint8_t value) {
  greyscale = static_cast<bool>(value & 0x1);
  show_leftmost_bg = static_cast<bool>((value >> 1) & 0x1);
  show_leftmost_sprites = static_cast<bool>((value >> 2) & 0x1);
  show_bg = static_cast<bool>((value >> 3) & 0x1);
  show_sprites = static_cast<bool>((value >> 4) & 0x1);
  emph_red = static_cast<bool>((value >> 5) & 0x1);
  emph_green = static_cast<bool>((value >> 6) & 0x1);
  emph_blue = static_cast<bool>((value >> 7) & 0x1);
}

uint8_t Ppu::ReadPpuStatus() {
  uint8_t value = (static_cast<uint8_t>(in_vblank) << 7) |
                  (static_cast<uint8_t>(sprite0_hit) << 6) |
                  (static_cast<uint8_t>(sprite_overflow) << 5) |
                  (last_write & 0x1F);

  in_vblank = false;
  reg_W = Toggle::Write1;

  return value;
}

// void Ppu::WritePpuScroll(uint8_t value) {
//   if (write_toggle) {
//     y_offset = static_cast<uint16_t>(value);
//   } else {
//     x_offset = static_cast<uint16_t>(value);
//   }

//   write_toggle = !write_toggle;
// }

void Ppu::WritePpuScroll(uint8_t value) {
  if (reg_W == Toggle::Write1) {
    reg_X = static_cast<uint16_t>(value & 0x3);
    reg_T |= static_cast<uint16_t>(value >> 3);
    reg_W = Toggle::Write2;
  } else {
    reg_T |= static_cast<uint16_t>(value & 0x7) << 12;
    reg_T |= static_cast<uint16_t>(value & 0xF8) << 2;
    reg_W = Toggle::Write1;
  }
}

// void Ppu::WritePpuAddr(uint8_t value) {
//   if (write_toggle) {
//     vram_addr = (vram_addr & 0xFF) | (static_cast<uint16_t>(value) << 8);
//   } else {
//     vram_addr = (vram_addr & 0xFF00) | static_cast<uint16_t>(value);
//   }

//   write_toggle = !write_toggle;
// }

void Ppu::WritePpuAddr(uint8_t value) {
  if (reg_W == Toggle::Write1) {
    reg_T &= 0x3FFF;
    reg_T |= static_cast<uint16_t>(value & 0x3F) << 8;
    reg_W = Toggle::Write2;
  } else {
    reg_T |= static_cast<uint16_t>(value);
    reg_V = reg_T;
    reg_W = Toggle::Write1;
  }
}

uint8_t Ppu::ReadPpuData(uint16_t addr) {
  uint8_t value;

  if (addr <= 0x3EFF) {
    value = read_buffer;
    reg_V += vram_addr_inc;
    read_buffer = ReadVram(reg_V);
  } else {
    value = ReadVram(reg_V);
    read_buffer = value;
    reg_V += vram_addr_inc;
  }

  return value;
}

void Ppu::WritePpuData(uint8_t value) {
  WriteVram(reg_V, value);
  reg_V += vram_addr_inc;
}

uint16_t Ppu::CalcNametableAddr(uint8_t x) {
  switch (x) {
    case 0:
      return 0x2000;
    case 1:
      return 0x2400;
    case 2:
      return 0x2800;
    default:
      return 0x2C00;
  }
}

uint8_t Ppu::ReadVram(uint16_t addr) {
  if (addr <= 0x0FFF) {
    return pattern_table0[addr];
  } else if (addr <= 0x1FFF) {
    return pattern_table1[addr - 0x1000];
  } else if (addr <= 0x23FF) {
    return nametable0[addr - 0x2000];
  } else if (addr <= 0x27FF) {
    return nametable1[addr - 0x2400];
  } else if (addr <= 0x2BFF) {
    return nametable2[addr - 0x2800];
  } else if (addr <= 0x2FFF) {
    return nametable3[addr - 0x2C00];
  } else if (addr <= 0x3F1F) {
    return palette_ram_idxs[addr - 0x3F00];
  } else {
    return 0x00;
  }
}

void Ppu::WriteVram(uint16_t addr, uint8_t value) {
  if (addr <= 0x0FFF) {
    pattern_table0[addr] = value;
  } else if (addr <= 0x1FFF) {
    pattern_table1[addr - 0x1000] = value;
  } else if (addr <= 0x23FF) {
    nametable0[addr - 0x2000] = value;
  } else if (addr <= 0x27FF) {
    nametable1[addr - 0x2400] = value;
  } else if (addr <= 0x2BFF) {
    nametable2[addr - 0x2800] = value;
  } else if (addr <= 0x2FFF) {
    nametable3[addr - 0x2C00] = value;
  } else if (addr <= 0x3F1F) {
    palette_ram_idxs[addr - 0x3F00] = value;
  }
}

}  // namespace graphics
