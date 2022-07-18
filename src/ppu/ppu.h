#ifndef SRC_PPU_PPU_H_
#define SRC_PPU_PPU_H_

#include <array>
#include <cstdint>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <vector>

#include "src/mappers/mapper.h"
#include "src/mirroring/mirroring.h"
#include "src/ppu/palette.h"
#include "src/ppu/state.h"

namespace graphics {

constexpr int SCREEN_WIDTH = 256;
constexpr int SCREEN_HEIGHT = 240;
constexpr int SCREEN_CHANNELS = 4;
constexpr int SCREEN_SIZE = SCREEN_HEIGHT * SCREEN_WIDTH * SCREEN_CHANNELS;
constexpr int PAT_TABLE_WIDTH = 128;
constexpr int PAT_TABLE_SIZE =
    PAT_TABLE_WIDTH * PAT_TABLE_WIDTH * SCREEN_CHANNELS;
constexpr int NAMETABLE_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_CHANNELS;
constexpr int NAMETABLE_ROWS = 30;
constexpr int NAMETABLE_COLS = 32;
constexpr int SPRITES_ROWS = 8;
constexpr int SPRITES_COLS = 8;
constexpr int SPRITE_BOX_WIDTH = 8 + 4;    // 2 pixels each for border & padding
constexpr int SPRITE_BOX_HEIGHT = 16 + 4;  // 2 pixels each for border & padding
constexpr int SPRITES_HEIGHT = SPRITES_ROWS * SPRITE_BOX_HEIGHT;
constexpr int SPRITES_WIDTH = SPRITES_COLS * SPRITE_BOX_WIDTH;
constexpr int SPRITES_SIZE = SPRITES_HEIGHT * SPRITES_WIDTH * SCREEN_CHANNELS;
constexpr int PALETTE_SIZE = 8 + 1;
constexpr int PALETTE_PADDING = 5;
constexpr int PALETTES_BOX_WIDTH = PALETTE_SIZE * 3 + 1;
constexpr int PALETTES_BOX_HEIGHT = PALETTE_SIZE + 1;
constexpr int PALETTES_WIDTH =
    (PALETTES_BOX_WIDTH + PALETTE_PADDING) * 4 + PALETTE_PADDING;
constexpr int PALETTES_HEIGHT =
    (PALETTES_BOX_HEIGHT + PALETTE_PADDING) * 2 + PALETTE_PADDING;
constexpr int PALETTES_SIZE =
    PALETTES_WIDTH * PALETTES_HEIGHT * SCREEN_CHANNELS;

constexpr uint16_t PALETTE_ADDRS[4][3] = {
    {0x3F01, 0x3F02, 0x3F03},
    {0x3F05, 0x3F06, 0x3F07},
    {0x3F09, 0x3F0A, 0x3F0B},
    {0x3F0D, 0x3F0E, 0x3F0F},
};

struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

class Ppu {
 public:
  Ppu(std::shared_ptr<mappers::Mapper> mapper);

  void Tick(uint64_t cycles);
  bool NmiOccured();
  void ClearNmi();
  void OamDmaWrite(uint8_t value);

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);

  void UpdatePatternTable(uint16_t table_offset = 0);
  void UpdateNametable(uint16_t addr);
  void UpdateSprites();
  void UpdatePalettes();

  void UseFceuxPalette();
  void UseNtscPalette();

  std::vector<uint8_t> screen;
  std::vector<uint8_t> pat_table1;
  std::vector<uint8_t> pat_table2;
  std::vector<uint8_t> nametable1;
  std::vector<uint8_t> nametable2;
  std::vector<uint8_t> nametable3;
  std::vector<uint8_t> nametable4;
  std::vector<uint8_t> sprites;
  std::vector<uint8_t> palettes;

 private:
  std::shared_ptr<mappers::Mapper> cartridge;

  /*****************************************************
    PPU state machine methods
  *****************************************************/

  void DataFetcherTick();
  // void SpriteEvalTick();
  void PixelTick();

  void VisibleOrPrerenderTick();
  void PostRenderTick();
  void VBlankTick();

  void EvalSprites();

  /*****************************************************
    Data read/write methods
  *****************************************************/

  uint8_t ReadVram(uint16_t addr);
  void WriteVram(uint16_t addr, uint8_t value);

  void WritePpuCtrl(uint8_t value);

  void WritePpuMask(uint8_t value);

  uint8_t ReadPpuStatus();

  void WritePpuScroll(uint8_t value);

  void WritePpuAddr(uint8_t value);

  uint8_t ReadPpuData();
  void WritePpuData(uint8_t value);

  /*****************************************************
    Helper methods
  *****************************************************/

  void LoadBg();
  void ReloadVertical();
  void ReloadHorizontal();
  void IncHorizontal();
  void IncVertical();
  void IncVram();
  void NextScanline();
  void NextDot();
  bool Disabled();
  void ShiftBgFifos();
  bool ShiftOnCycle();
  void ShiftSpriteFifos(int i);
  Color GetRgb(uint8_t palette, uint8_t value, uint16_t offset);
  uint8_t GetSpriteValue(int i);

  uint16_t CalcNametableAddr(uint8_t x);

  void UpdateNmi();

  /*****************************************************
    PPU state and screen data
  *****************************************************/

  // PPU state variants
  ScanlineType scanline_type = ScanlineType::PreRender;
  CycleType cycle_type = CycleType::Cycle0;
  // SpriteFetchType sprite_fetch_type = SpriteFetchType::SecondaryOamInit0;

  // PPU state info
  uint64_t dot = 0;
  uint64_t line = 261;
  uint64_t frame = 1;

  // Bg shift registers
  uint16_t pattern_queue1;
  uint16_t pattern_queue2;
  uint16_t palette_queue1;
  uint16_t palette_queue2;

  /*---------------------------------------------------
    Sprite registers
  ---------------------------------------------------*/
  // shift registers
  uint8_t sprite_queues1[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  uint8_t sprite_queues2[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  // latches
  uint8_t sprite_attrs[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  // counters
  uint8_t sprite_counters[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  // sprite numbers
  uint8_t sprite_nums[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  /*****************************************************
    Memory and registers
  *****************************************************/

  /*---------------------------------------------------
    PPU state
  ---------------------------------------------------*/
  std::array<uint8_t, 32> palette_ram_idxs;
  std::array<uint8_t, 256> obj_attr_memory;
  std::array<uint8_t, 32> secondary_oam;

  /*---------------------------------------------------
    PPU Registers
  ---------------------------------------------------*/
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
  uint16_t emph_red = 0;
  uint16_t emph_green = 0;
  uint16_t emph_blue = 0;

  /* PPUSTATUS 0x2002 */
  bool sprite_overflow = false;
  bool sprite0_hit = false;
  bool in_vblank = false;

  /* OAMADDR 0x2003 */
  uint8_t oam_addr = 0x0;

  /*---------------------------------------------------
    Internal registers
  ---------------------------------------------------*/

  // current VRAM address
  uint16_t reg_V = 0x0000;
  // temporary VRAM address
  uint16_t reg_T = 0x0000;
  // fine X scroll
  uint16_t reg_X = 0x0000;
  // first or second write toggle
  Toggle reg_W = Toggle::Write1;

  /*---------------------------------------------------
    Internal state
  ---------------------------------------------------*/

  uint8_t sprite_idx = 0x0;
  uint8_t secondary_oam_idx = 0x0;
  uint8_t sprite_palette = 0x0;
  bool sprite0_on_scanline = false;
  int num_sprites_on_line = 0;

  // sprite fetching
  uint16_t sprite_tile_idx = 0x00;
  uint16_t sprite_addr = 0x00;

  uint8_t last_write = 0x00;
  uint8_t read_buffer = 0x00;

  // nametable fetching
  uint16_t tile_addr = 0x0;
  uint16_t attr_addr = 0x0;
  uint8_t attr_byte = 0x0;
  uint8_t palette_latch = 0x0;
  uint16_t nametable_byte = 0x0;
  uint16_t bg_addr = 0x0;
  uint8_t bg_tile_low = 0x0;
  uint8_t bg_tile_high = 0x0;
  // NMI
  bool nmi_pending = false;

  /*---------------------------------------------------
    Selected Palette
  ---------------------------------------------------*/
  std::reference_wrapper<const std::array<uint8_t, PALETTE_ARRAY_SIZE>>
      selected_palette;
};

}  // namespace graphics

#endif  // SRC_PPU_PPU_H_
