#include "ppu.h"

#include <cstdint>
#include <memory>

#include "src/mappers/mapper.h"
#include "src/mirroring/mirroring.h"
#include "src/ppu/state.h"

namespace graphics {

Ppu::Ppu(std::shared_ptr<mappers::Mapper> mapper)
    : cartridge(std::move(mapper)),
      palette_ram_idxs(),
      obj_attr_memory(),
      secondary_oam() {}

void Ppu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    PixelTick();
    DataFetcherTick();
    SpriteEvalTick();

    dot = dot == 340 ? 0 : dot + 1;

    cycles--;
  }
}

void Ppu::DataFetcherTick() {
  switch (scanline_type) {
    case ScanlineType::PreRender:
      return VisibleOrPrerenderTick();
    case ScanlineType::Visible:
      return VisibleOrPrerenderTick();
    case ScanlineType::PostRender:
      return PostRenderTick();
    case ScanlineType::VBlank:
      return VBlankTick();
  }
}

void Ppu::VisibleOrPrerenderTick() {
  switch (cycle_type) {
    /* Cycle/Dot 0 */
    /******************************************************************/
    case CycleType::Cycle0: {
      cycle_type = CycleType::NametableByte0;
      return;
    }
    /* Cycles/Dots 1-256 and 321-336 */
    /******************************************************************/
    case CycleType::NametableByte0: {
      if (dot == 1 && scanline_type == ScanlineType::PreRender) {
        in_vblank = false;
        UpdateNmi();
        sprite_overflow = false;
        sprite0_hit = false;
      }
      // Get tile address
      tile_addr = 0x2000 | (reg_V & 0x0FFF);

      cycle_type = CycleType::NametableByte1;
      return;
    }
    /******************************************************************/
    case CycleType::NametableByte1: {
      // Get nametable byte
      nametable_byte = static_cast<uint16_t>(ReadVram(tile_addr));

      cycle_type = CycleType::AttrByte0;
      return;
    }
    /******************************************************************/
    case CycleType::AttrByte0: {
      // Get attribute address
      attr_addr = 0x23C0 | (reg_V & 0x0C00) | ((reg_V >> 4) & 0x38) |
                  ((reg_V >> 2) & 0x07);

      cycle_type = CycleType::AttrByte1;
      return;
    }
    /******************************************************************/
    case CycleType::AttrByte1: {
      // TODO
      cycle_type = CycleType::PatternTileLow0;
      return;
    }
    /******************************************************************/
    case CycleType::PatternTileLow0: {
      uint16_t fine_y = (reg_V >> 12) & 0x7;
      bg_addr = fine_y | (nametable_byte << 4) | pattern_table_addr;

      cycle_type = CycleType::PatternTileLow1;
      return;
    }
    /******************************************************************/
    case CycleType::PatternTileLow1: {
      bg_tile_low = ReadVram(bg_addr);

      cycle_type = CycleType::PatternTileHigh0;
      return;
    }
    /******************************************************************/
    case CycleType::PatternTileHigh0: {
      bg_addr |= 0x8;

      cycle_type = CycleType::PatternTileHigh1;
      return;
    }
    /******************************************************************/
    case CycleType::PatternTileHigh1: {
      bg_tile_high = ReadVram(bg_addr);
      // load data into shift registers
      ShiftBg();
      // increment reg_V hori(v)
      IncHorizontal();
      if (dot == 256) {
        // increment reg_V vert(v) at dot 256
        IncVertical();
        cycle_type = CycleType::GarbageByte0;
      } else if (dot == 336) {
        cycle_type == CycleType::FirstUnkByte0;
      } else {
        cycle_type = CycleType::NametableByte0;
      }
      return;
    }
    /* Cycles/Dots 257-320 */
    /******************************************************************/
    case CycleType::GarbageByte0: {
      // hori(v) := hori(t)
      ReloadHorizontal();
      cycle_type = CycleType::GarbageByte1;
      return;
    }
    /******************************************************************/
    case CycleType::GarbageByte1: {
      cycle_type = CycleType::GarbageByte2;
      return;
    }
    /******************************************************************/
    case CycleType::GarbageByte2: {
      cycle_type = CycleType::GarbageByte3;
      return;
    }
    /******************************************************************/
    case CycleType::GarbageByte3: {
      cycle_type = CycleType::NextSpriteTileLow0;
      return;
    }
    /******************************************************************/
    case CycleType::NextSpriteTileLow0: {
      cycle_type = CycleType::NextSpriteTileLow1;
      return;
    }
    /******************************************************************/
    case CycleType::NextSpriteTileLow1: {
      cycle_type = CycleType::NextSpriteTileHigh0;
      return;
    }
    /******************************************************************/
    case CycleType::NextSpriteTileHigh0: {
      cycle_type = CycleType::NametableByte0;
      return;
    }
    /* Cycles/Dots 337-340 */
    /******************************************************************/
    case CycleType::FirstUnkByte0: {
      cycle_type = CycleType::FirstUnkByte1;
      return;
    }
    /******************************************************************/
    case CycleType::FirstUnkByte1: {
      cycle_type = CycleType::SecondUnkByte0;
      return;
    }
    /******************************************************************/
    case CycleType::SecondUnkByte0: {
      cycle_type = CycleType::SecondUnkByte1;
      return;
    }
    /******************************************************************/
    case CycleType::SecondUnkByte1: {
      // line++;

      // if (line == 240) {
      //   if (scanline_type == ScanlineType::Visible) {
      //     scanline_type = ScanlineType::PostRender;
      //   } else if (scanline_type == ScanlineType::PreRender) {
      //     scanline_type = ScanlineType::Visible;
      //   }
      // }
      NextScanline();
      return;
    }
  }

  // continuously reload vert(v) during cycles 280-304 of the pre-render line
  if (scanline_type == ScanlineType::PreRender && dot >= 280 && dot <= 304 &&
      show_bg) {
    // vert(v) := vert(t)
    ReloadVertical();
  }
}

void Ppu::ShiftBg() {
  for (int i = 7; i >= 0; i--) {
    pattern_queue1.push((bg_tile_low >> i) & 0x1);
    pattern_queue1.push((bg_tile_high >> i) & 0x1);
  }
}

void Ppu::ReloadVertical() {
  reg_V &= 0x7C1F;
  reg_V |= (reg_T & ~0x7C1F);
}

void Ppu::ReloadHorizontal() {
  reg_V &= 0x7BE0;
  reg_V |= (reg_T & ~0x7BE0);
}

void Ppu::IncHorizontal() {
  if ((reg_V & 0x1F) == 0x1F) {
    reg_V &= 0xFFE0;
    reg_V ^= 0x0400;
  } else {
    reg_V++;
  }
}

void Ppu::IncVertical() {
  if ((reg_V & 0x7000) != 0x7000) {
    reg_V += 0x1000;
  } else {
    reg_V &= ~0x7000;
    uint16_t coarse_y = (reg_V & 0x3E0) >> 5;
    if (coarse_y == 0x1D) {
      coarse_y = 0;
      reg_V ^= 0x800;
    } else if (coarse_y == 0x1F) {
      coarse_y = 0;
    } else {
      coarse_y++;
    }

    reg_V = (reg_V & ~0x3E0) | (coarse_y << 5);
  }
}

void Ppu::PostRenderTick() {
  if (dot == 340) {
    NextScanline();
  }
}

void Ppu::VBlankTick() {
  if (line == 241 and dot == 1) {
    in_vblank = true;
    UpdateNmi();
  }

  if (dot == 340) {
    NextScanline();
  }
}

void Ppu::NextScanline() {
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

bool Ppu::NmiOccured() { return nmi_occured; }

void Ppu::ClearNmi() { nmi_occured = false; }

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
  sprite_table_addr = static_cast<bool>(((value >> 3) & 0x1)) ? 0x1000 : 0x0;
  pattern_table_addr = static_cast<bool>(((value >> 4) & 0x1)) ? 0x1000 : 0x0;
  long_sprites = static_cast<bool>((value >> 5) & 0x1);
  ppu_select = static_cast<bool>((value >> 6) & 0x1);
  generate_vblank_nmi = static_cast<bool>((value >> 7) & 0x1);
  UpdateNmi();

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
  UpdateNmi();
  reg_W = Toggle::Write1;

  return value;
}

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

void Ppu::UpdateNmi() { nmi_occured = in_vblank && generate_vblank_nmi; }

uint8_t Ppu::ReadVram(uint16_t addr) {
  if (addr <= 0x2FFF) {
    return cartridge->PpuRead(addr);
  } else if (addr <= 0x3F1F) {
    return palette_ram_idxs[addr - 0x3F00];
  } else {
    return 0x00;
  }
}

void Ppu::WriteVram(uint16_t addr, uint8_t value) {
  if (addr <= 0x2FFF) {
    cartridge->PpuWrite(addr, value);
  } else if (addr <= 0x3F1F) {
    palette_ram_idxs[addr - 0x3F00] = value;
  }
}

}  // namespace graphics
