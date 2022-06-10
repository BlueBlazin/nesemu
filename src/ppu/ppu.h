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

  uint8_t ReadVram(uint16_t addr);
  void WriteVram(uint16_t addr, uint8_t value);

  void WritePpuCtrl(uint8_t value);

  void WritePpuMask(uint8_t value);

  uint8_t ReadPpuStatus();

  void WritePpuScroll(uint8_t value);

  void WritePpuAddr(uint8_t value);

  uint8_t ReadPpuData(uint16_t addr);
  void WritePpuData(uint8_t value);

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

  /* PPUCTRL 0x2000 */
  uint16_t base_nametable_addr = 0x2000;
  uint16_t vram_addr_inc = 1;
  uint16_t sprite_table_addr = 0x0000;
  uint16_t pattern_table_addr = 0x0000;
  bool long_sprites = false;
  bool ppu_select = false;
  bool generate_vblank_nmi = false;

  /* PPUMASK 0x2001 */
  bool greyscale = false;
  bool show_leftmost_bg = true;
  bool show_leftmost_sprites = true;
  bool show_bg = true;
  bool show_sprites = true;
  bool emph_red = false;
  bool emph_green = false;
  bool emph_blue = false;

  /* PPUSTATUS 0x2002 */
  uint8_t last_lo_nibble = 0x0;
  bool sprite_overflow = false;
  bool sprite0_hit = false;
  bool in_vblank = false;

  /* OAMADDR 0x2003 */
  uint16_t oam_addr = 0x0000;

  /* OAMDATA 0x2004 */
  // virtual

  /* PPUSCROLL 0x2005 */
  uint16_t x_offset = 0x0;
  uint16_t y_offset = 0x0;

  /* PPUSADDR 0x2006 */
  uint16_t vram_addr = 0x0000;

  /* PPUDATA 0x2007 */
  uint16_t vram_data = 0x0000;

  /* Internal */
  bool write_toggle = false;
  uint8_t last_write = 0x0;
  uint8_t read_buffer = 0x0;

  uint8_t oam_dma = 0x0;

  uint8_t x_scroll = 0x0;
  uint8_t y_scroll = 0x0;

  bool ppuaddr_in_use = false;

  std::queue<uint8_t> pat_queue1;
  std::queue<uint8_t> pat_queue2;
  std::queue<uint8_t> palette_queue1;
  std::queue<uint8_t> palette_queue2;

  uint16_t CalcNametableAddr(uint8_t x);
};

}  // namespace graphics

#endif  // SRC_PPU_PPU_
