#include "ppu.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include "src/mappers/mapper.h"
#include "src/mirroring/mirroring.h"
#include "src/ppu/state.h"

namespace graphics {

Ppu::Ppu(std::shared_ptr<mappers::Mapper> mapper)
    : screen(SCREEN_SIZE, 0),
      pat_table1(PAT_TABLE_SIZE, 0),
      pat_table2(PAT_TABLE_SIZE, 0),
      nametable1(NAMETABLE_SIZE, 0),
      nametable2(NAMETABLE_SIZE, 0),
      nametable3(NAMETABLE_SIZE, 0),
      nametable4(NAMETABLE_SIZE, 0),
      cartridge(std::move(mapper)),
      pattern_queue1(),
      pattern_queue2(),
      palette_queue1(),
      palette_queue2(),
      palette_ram_idxs(),
      obj_attr_memory(),
      secondary_oam() {
  // Initialize screen
  for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH * SCREEN_CHANNELS; i++) {
    screen[i] = 0x00;
  }
}

void Ppu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    PixelTick();
    DataFetcherTick();
    SpriteEvalTick();

    NextDot();
    cycles--;
  }
}

void Ppu::SpriteEvalTick() {
  // TODO
}

void Ppu::PixelTick() {
  if (Disabled() || scanline_type != ScanlineType::Visible || dot == 0 ||
      dot > 256) {
    // PPU is disabled, don't shift pixels
    return;
  }

  // get correct bits using fine X scroll
  uint8_t bg_lo = static_cast<uint8_t>(pattern_queue1 >> (15 - reg_X));
  uint8_t bg_hi = static_cast<uint8_t>(pattern_queue2 >> (15 - reg_X));

  uint8_t bg = (bg_hi << 1) | bg_lo;

  int idx = (line * SCREEN_WIDTH + dot) * SCREEN_CHANNELS;

  screen[idx + 0] = bg * 85;
  screen[idx + 1] = bg * 85;
  screen[idx + 2] = bg * 85;
  screen[idx + 3] = 0xFF;
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
  if (Disabled()) {
    // PPU disabled, don't do anything
    return;
  }

  if (ShiftOnCycle()) {
    ShiftBg();
  }

  switch (cycle_type) {
    /* Cycle/Dot 0 */
    /******************************************************************/
    case CycleType::Cycle0: {
      cycle_type = CycleType::NametableByte0;
      break;
    }
    /* Cycles/Dots 1-256 and 321-336 */
    /******************************************************************/
    case CycleType::NametableByte0: {
      // Get tile address
      tile_addr = 0x2000 | (reg_V & 0x0FFF);

      cycle_type = CycleType::NametableByte1;
      break;
    }
    /******************************************************************/
    case CycleType::NametableByte1: {
      // Get nametable byte
      nametable_byte = static_cast<uint16_t>(ReadVram(tile_addr));
      cycle_type = CycleType::AttrByte0;
      break;
    }
    /******************************************************************/
    case CycleType::AttrByte0: {
      // Get attribute address
      attr_addr = 0x23C0 | (reg_V & 0x0C00) | ((reg_V >> 4) & 0x38) |
                  ((reg_V >> 2) & 0x07);

      cycle_type = CycleType::AttrByte1;
      break;
    }
    /******************************************************************/
    case CycleType::AttrByte1: {
      // TODO
      cycle_type = CycleType::PatternTileLow0;
      break;
    }
    /******************************************************************/
    case CycleType::PatternTileLow0: {
      uint16_t fine_y = (reg_V >> 12) & 0x7;
      bg_addr = (pattern_table_addr << 12) | (nametable_byte << 4) | fine_y;
      cycle_type = CycleType::PatternTileLow1;
      break;
    }
    /******************************************************************/
    case CycleType::PatternTileLow1: {
      bg_tile_low = ReadVram(bg_addr);

      cycle_type = CycleType::PatternTileHigh0;
      break;
    }
    /******************************************************************/
    case CycleType::PatternTileHigh0: {
      bg_addr |= 0x8;

      cycle_type = CycleType::PatternTileHigh1;
      break;
    }
    /******************************************************************/
    case CycleType::PatternTileHigh1: {
      bg_tile_high = ReadVram(bg_addr);
      // load data into shift registers
      LoadBg();
      IncHorizontal();
      // increment reg_V hori(v)
      if (dot == 256) {
        IncVertical();
        cycle_type = CycleType::GarbageByte0;
      } else if (dot == 336) {
        cycle_type = CycleType::FirstUnkByte0;
      } else {
        cycle_type = CycleType::NametableByte0;
      }
      break;
    }
    /* Cycles/Dots 257-320 */
    /******************************************************************/
    case CycleType::GarbageByte0: {
      // hori(v) := hori(t)
      if (dot == 257) {
        ReloadHorizontal();
      }
      cycle_type = CycleType::GarbageByte1;
      break;
    }
    /******************************************************************/
    case CycleType::GarbageByte1: {
      cycle_type = CycleType::GarbageByte2;
      break;
    }
    /******************************************************************/
    case CycleType::GarbageByte2: {
      cycle_type = CycleType::GarbageByte3;
      break;
    }
    /******************************************************************/
    case CycleType::GarbageByte3: {
      cycle_type = CycleType::NextSpriteTileLow0;
      break;
    }
    /******************************************************************/
    case CycleType::NextSpriteTileLow0: {
      cycle_type = CycleType::NextSpriteTileLow1;
      break;
    }
    /******************************************************************/
    case CycleType::NextSpriteTileLow1: {
      cycle_type = CycleType::NextSpriteTileHigh0;
      break;
    }
    /******************************************************************/
    case CycleType::NextSpriteTileHigh0: {
      cycle_type = CycleType::NextSpriteTileHigh1;
      break;
    }
    /******************************************************************/
    case CycleType::NextSpriteTileHigh1: {
      if (dot == 320) {
        cycle_type = CycleType::NametableByte0;
      } else {
        cycle_type = CycleType::GarbageByte0;
      }
      break;
    }
    /* Cycles/Dots 337-340 */
    /******************************************************************/
    case CycleType::FirstUnkByte0: {
      cycle_type = CycleType::FirstUnkByte1;
      break;
    }
    /******************************************************************/
    case CycleType::FirstUnkByte1: {
      cycle_type = CycleType::SecondUnkByte0;
      break;
    }
    /******************************************************************/
    case CycleType::SecondUnkByte0: {
      cycle_type = CycleType::SecondUnkByte1;
      break;
    }
    /******************************************************************/
    case CycleType::SecondUnkByte1: {
      break;
    }
  }

  // continuously reload vert(v) during cycles 280-304 of the pre-render line
  if (scanline_type == ScanlineType::PreRender && dot >= 280 && dot <= 304) {
    // vert(v) := vert(t)
    ReloadVertical();
  }
}

void Ppu::LoadBg() {
  pattern_queue1 &= 0xFF00;
  pattern_queue2 &= 0xFF00;
  pattern_queue1 |= static_cast<uint16_t>(bg_tile_low);
  pattern_queue2 |= static_cast<uint16_t>(bg_tile_high);
}

void Ppu::ShiftBg() {
  pattern_queue1 = pattern_queue1 << 1;
  pattern_queue2 = pattern_queue2 << 1;
}

bool Ppu::ShiftOnCycle() {
  return (
      scanline_type == ScanlineType::Visible &&
          ((dot >= 1 && dot <= 256) || (dot >= 321 && dot <= 336)) ||
      (scanline_type == ScanlineType::PreRender && (dot >= 321 && dot <= 336)));
}

void Ppu::ReloadVertical() {
  reg_V &= 0x041F;
  reg_V |= (reg_T & 0x7BE0);
}

void Ppu::ReloadHorizontal() {
  reg_V &= 0x7BE0;
  reg_V |= (reg_T & 0x041F);
}

void Ppu::IncHorizontal() {
  if ((reg_V & 0x1F) == 0x1F) {
    reg_V &= ~0x001F;
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
    uint16_t y = (reg_V & 0x03E0) >> 5;
    if (y == 29) {
      y = 0;
      reg_V ^= 0x0800;
    } else if (y == 31) {
      y = 0;
    } else {
      y += 1;
    }
    reg_V = (reg_V & ~0x03E0) | (y << 5);
  }
}

void Ppu::IncVram() { reg_V = (reg_V + vram_addr_inc) & 0x7FFF; }

void Ppu::PostRenderTick() {}

void Ppu::VBlankTick() {
  if (line == 241 and dot == 1) {
    in_vblank = true;
    UpdateNmi();
  }
}

void Ppu::NextScanline() {
  line++;

  if (line == 262) {
    line = 0;
    frame++;
  }

  if (line == 0) {
    scanline_type = ScanlineType::Visible;
  } else if (line == 240) {
    scanline_type = ScanlineType::PostRender;
  } else if (line == 241) {
    scanline_type = ScanlineType::VBlank;
  } else if (line == 261) {
    scanline_type = ScanlineType::PreRender;
  }

  if (scanline_type == ScanlineType::Visible ||
      scanline_type == ScanlineType::PreRender) {
    // restart state machine
    cycle_type = CycleType::Cycle0;
  }
}

void Ppu::NextDot() {
  dot++;

  if (dot == 1 && scanline_type == ScanlineType::PreRender) {
    in_vblank = false;
    UpdateNmi();
    sprite_overflow = false;
    sprite0_hit = false;
  } else if (dot == 341) {
    dot = 0;
    NextScanline();
  }
}

bool Ppu::Disabled() { return !show_bg && !show_sprites; }

bool Ppu::NmiOccured() { return nmi_pending; }

void Ppu::ClearNmi() { nmi_pending = false; }

void Ppu::OamDmaWrite(uint8_t value) { obj_attr_memory[oam_addr++] = value; }

uint8_t Ppu::Read(uint16_t addr) {
  switch (addr) {
    case 0x2002:
      return ReadPpuStatus();
    case 0x2004:
      return obj_attr_memory[oam_addr];
    case 0x2007:
      return ReadPpuData();
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
      oam_addr = value;
      break;
    case 0x2004:
      obj_attr_memory[oam_addr++] = value;
      break;
    case 0x2005:
      WritePpuScroll(value);
      break;
    case 0x2006:
      WritePpuAddr(value);
      break;
    case 0x2007:
      WritePpuData(value);
      break;
  }

  last_write = value;
}

void Ppu::WritePpuCtrl(uint8_t value) {
  base_nametable_addr = CalcNametableAddr(value & 0x3);
  vram_addr_inc = static_cast<bool>((value >> 2) & 0x1) ? 32 : 1;
  sprite_table_addr = static_cast<bool>(((value >> 3) & 0x1)) ? 0x1000 : 0x0;
  pattern_table_addr = static_cast<uint16_t>((value >> 4) & 0x1);
  long_sprites = static_cast<bool>((value >> 5) & 0x1);
  ppu_select = static_cast<bool>((value >> 6) & 0x1);
  generate_vblank_nmi = static_cast<bool>((value >> 7) & 0x1);
  UpdateNmi();

  reg_T &= 0x73FF;
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
    reg_T &= 0x7FE0;
    reg_T |= static_cast<uint16_t>(value >> 3);
    reg_W = Toggle::Write2;
  } else {
    reg_T &= 0xC1F;
    reg_T |= static_cast<uint16_t>(value & 0x7) << 12;
    reg_T |= static_cast<uint16_t>(value & 0xF8) << 2;
    reg_W = Toggle::Write1;
  }
}

void Ppu::WritePpuAddr(uint8_t value) {
  if (reg_W == Toggle::Write1) {
    reg_T &= 0xFF;
    reg_T |= static_cast<uint16_t>(value & 0x3F) << 8;
    reg_W = Toggle::Write2;
  } else {
    reg_T &= 0x7F00;
    reg_T |= static_cast<uint16_t>(value);
    reg_V = reg_T;
    reg_W = Toggle::Write1;
  }
}

uint8_t Ppu::ReadPpuData() {
  uint8_t value;

  if (reg_V <= 0x3EFF) {
    value = read_buffer;
    IncVram();
    read_buffer = ReadVram(reg_V);
  } else {
    value = ReadVram(reg_V);
    read_buffer = value;
    IncVram();
  }

  return value;
}

void Ppu::WritePpuData(uint8_t value) {
  WriteVram(reg_V, value);
  IncVram();
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

void Ppu::UpdateNmi() {
  nmi_pending = nmi_pending || (in_vblank && generate_vblank_nmi);
}

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
  if (addr <= 0x1FFF) {
    cartridge->PpuWrite(addr, value);
  } else if (addr <= 0x2FFF) {
    cartridge->PpuWrite(addr, value);
  } else if (addr <= 0x3F1F) {
    palette_ram_idxs[addr - 0x3F00] = value;
  }
}

}  // namespace graphics
