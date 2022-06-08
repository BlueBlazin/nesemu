#ifndef SRC_PPU_PPU_
#define SRC_PPU_PPU_

#include <array>
#include <cstdint>
#include <queue>

namespace graphics {

enum class ScanlineType {
  PreRender,
  Visible,
  SpriteDataFetch,
  TileDataFetch,
  UnkFetch,
  PostRender,
  VBlank,
};

enum class CycleType {
  /* Cycles 0 (idle) */
  Cycle0,
  /* Cycles 1-256 */
  NametableByte0,
  NametableByte1,
  AttrByte0,
  AttrByte1,
  PatternTileLow0,
  PatternTileLow1,
  PatternTileHigh0,
  PatternTileHigh1,
  /* Cycles 257-320 */
  GarbageByte0,
  GarbageByte1,
  GarbageByte2,
  GarbageByte3,
  NextSpriteTileLow0,
  NextSpriteTileLow1,
  NextSpriteTileHigh0,
  NextSpriteTileHigh1,
  /* Cycles 321-336 */
  NextNametableByte0,
  NextNametableByte1,
  NextAttrByte0,
  NextAttrByte1,
  NextPatternTileLow0,
  NextPatternTileLow1,
  NextPatternTileHigh0,
  NextPatternTileHigh1,
  /* Cycles 337-340 */
  FirstUnkByte0,
  FirstUnkByte1,
  SecondUnkByte0,
  SecondUnkByte1,
};

class PpuCtr {
 public:
  uint8_t AsByte() {
    return (static_cast<uint8_t>(vblank_nmi) << 7) |
           (static_cast<uint8_t>(select) << 6) |
           (static_cast<uint8_t>(long_sprites) << 5) |
           (static_cast<uint8_t>(bg_addr == 0 ? 0 : 1) << 4) |
           (static_cast<uint8_t>(sprite_addr == 0 ? 0 : 1) << 3) |
           (static_cast<uint8_t>(addr_inc == 1 ? 0 : 1) << 2) |
           static_cast<uint8_t>((base_addr - 0x2000) >> 10);
  }

  uint16_t base_addr = 0x2000;
  uint16_t addr_inc = 1;
  uint16_t sprite_addr = 0x0000;
  uint16_t bg_addr = 0x0000;
  bool long_sprites = false;
  bool select = false;
  bool vblank_nmi = false;
};

class Ppu {
 public:
  Ppu();

  void Tick(uint64_t cycles);

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);

 private:
  void DataFetcherTick();
  void SpriteEvalTick();
  void PixelTick();

  void VisibleScanlineTick();

  /* PPU register utility methods */

  // PPUCTRL
  uint16_t BaseNtAddr();
  uint16_t VramAddrIncrement();
  uint16_t SpritePtAddr();
  uint16_t BgPtAddr();

  ScanlineType scanline_type = ScanlineType::PreRender;
  CycleType cycle_type = CycleType::Cycle0;

  // PPU state
  uint64_t state = 0;
  // 0x0000 - 0x0FFF
  std::array<uint8_t, 4096> pattern_table0;
  // 0x1000 - 0x1FFF
  std::array<uint8_t, 4096> pattern_table1;
  // 0x2000 - 0x23FF
  std::array<uint8_t, 1024> nametable0;
  // 0x2400 - 0x27FF
  std::array<uint8_t, 1024> nametable1;
  // 0x2800 - 0x2BFF
  std::array<uint8_t, 1024> nametable2;
  // 0x2C00 - 0x2FFF
  std::array<uint8_t, 1024> nametable3;
  // 0x3F00 - 0x3F1F
  std::array<uint8_t, 32> palette_ram_idxs;

  std::array<uint8_t, 256> obj_attr_memory;

  uint64_t line = 0;
  uint64_t clock = 0;
  uint64_t frame = 0;

  uint8_t ppu_ctrl = 0x0;
  uint8_t ppu_mask = 0x0;
  uint8_t ppu_status = 0x0;
  uint8_t oam_addr = 0x0;
  uint8_t oam_data = 0x0;
  uint8_t ppu_scroll = 0x0;
  uint8_t ppu_addr = 0x0;
  uint8_t ppu_data = 0x0;
  uint8_t oam_dma = 0x0;

  uint8_t x_scroll = 0x0;
  uint8_t y_scroll = 0x0;

  bool ppuaddr_in_use = false;

  std::queue<uint8_t> pat_queue1;
  std::queue<uint8_t> pat_queue2;
  std::queue<uint8_t> palette_queue1;
  std::queue<uint8_t> palette_queue2;
};

}  // namespace graphics

#endif  // SRC_PPU_PPU_
