#ifndef SRC_PPU_PPU_
#define SRC_PPU_PPU_

#include <array>
#include <cstdint>
#include <memory>
#include <queue>

#include "src/mappers/mapper.h"
#include "src/mirroring/mirroring.h"
#include "src/ppu/state.h"

namespace graphics {

class Ppu {
 public:
  Ppu(std::shared_ptr<mappers::Mapper> mapper);

  void Tick(uint64_t cycles);
  bool NmiOccured();
  void ClearNmi();

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);

 private:
  /*****************************************************
    PPU state machine methods
  *****************************************************/

  void DataFetcherTick();
  void SpriteEvalTick();
  void PixelTick();

  void VisibleOrPrerenderTick();
  void PostRenderTick();
  void VBlankTick();

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

  uint8_t ReadPpuData(uint16_t addr);
  void WritePpuData(uint8_t value);

  /*****************************************************
    Helper methods
  *****************************************************/

  void ShiftBg();

  void ReloadVertical();
  void ReloadHorizontal();
  void IncHorizontal();
  void IncVertical();
  void NextScanline();

  uint16_t CalcNametableAddr(uint8_t x);

  void UpdateNmi();

  /*****************************************************
    PPU state and screen data
  *****************************************************/

  // PPU state variants
  ScanlineType scanline_type = ScanlineType::PreRender;
  CycleType cycle_type = CycleType::Cycle0;

  // PPU state info
  uint64_t dot = 0;
  uint64_t line = 0;
  uint64_t frame = 1;

  // Shift registers
  std::queue<uint8_t> pattern_queue1;
  std::queue<uint8_t> pattern_queue2;
  std::queue<uint8_t> palette_queue1;
  std::queue<uint8_t> palette_queue2;

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

  /* PPUSCROLL 0x2005 */
  uint16_t x_offset = 0x0;
  uint16_t y_offset = 0x0;

  /* PPUSADDR 0x2006 */
  // uint16_t vram_addr = 0x0000;

  /* PPUDATA 0x2007 */
  uint16_t vram_data = 0x0000;

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
  std::shared_ptr<mappers::Mapper> cartridge;

  uint8_t last_write = 0x00;
  uint8_t read_buffer = 0x00;
  uint8_t oam_dma = 0x00;

  // nametable fetching
  uint16_t tile_addr = 0x0;
  uint16_t attr_addr = 0x0;
  uint16_t nametable_byte = 0x0;
  uint16_t bg_addr = 0x0;
  uint8_t bg_tile_low = 0x0;
  uint8_t bg_tile_high = 0x0;
  // NMI
  bool nmi_occured = false;
};

}  // namespace graphics

#endif  // SRC_PPU_PPU_
