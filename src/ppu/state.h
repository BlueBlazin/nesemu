#ifndef SRC_PPU_STATE_H_
#define SRC_PPU_STATE_H_

#include <iostream>

namespace graphics {

enum class ScanlineType {
  PreRender,
  Visible,
  PostRender,
  VBlank,
};

std::ostream& operator<<(std::ostream& os, const ScanlineType& scanline_type);

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

std::ostream& operator<<(std::ostream& os, const CycleType& cycle_type);

enum class Toggle {
  Write1,
  Write2,
};

}  // namespace graphics

#endif  // SRC_PPU_STATE_H_
