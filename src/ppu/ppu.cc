#include "ppu.h"

#include <cstdint>

namespace graphics {

void Ppu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    DataFetcherTick();
    PixelTick();
    SpriteEvalTick();

    cycles--;
  }
}

void Ppu::DataFetcherTick() {
  switch (scanline_type) {
    case ScanlineType::PreRender:
      break;
    case ScanlineType::Visible:
      return VisibleScanlineTick();
    case ScanlineType::SpriteDataFetch:
      break;
    case ScanlineType::TileDataFetch:
      break;
    case ScanlineType::UnkFetch:
      break;
    case ScanlineType::PostRender:
      break;
    case ScanlineType::VBlank:
      break;
  }
}

void Ppu::VisibleScanlineTick() {
  switch (cycle_type) {
    //
  }
}

uint8_t Ppu::Read(uint16_t addr) {
  if (addr <= 0x3FFF) {
    switch ((addr - 0x2000) % 8) {
      case 2:
        return ppu_status;
      case 4:
        return oam_data;
      case 7:
        return ppu_data;
    }
  }
}

void Ppu::Write(uint16_t addr, uint8_t value) {
  if (addr <= 0x3FFF) {
    switch ((addr - 0x2000) % 8) {
      case 0:
        ppu_ctrl = value;
        break;
      case 1:
        ppu_mask = value;
        break;
      case 3:
        oam_addr = value;
        break;
      case 4:
        oam_data = value;
        break;
      case 5:
        ppu_scroll = value;
        break;
      case 6:
        ppu_addr = value;
        break;
      case 7:
        ppu_data = value;
        break;
    }
  } else if (addr == 0x4014) {
    oam_dma = value;
  }
}

}  // namespace graphics
