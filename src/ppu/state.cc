#include "src/ppu/state.h"

#include <iostream>
#include <string>

namespace graphics {

std::ostream& operator<<(std::ostream& os, const ScanlineType& scanline_type) {
  switch (scanline_type) {
    case ScanlineType::PreRender:
      return os << "PreRender";
    case ScanlineType::Visible:
      return os << "Visible";
    case ScanlineType::PostRender:
      return os << "PostRender";
    case ScanlineType::VBlank:
      return os << "VBlank";
  }
}

std::ostream& operator<<(std::ostream& os, const CycleType& cycle_type) {
  switch (cycle_type) {
    case CycleType::Cycle0:
      return os << "Cycle0";
    case CycleType::NametableByte0:
      return os << "NametableByte0";
    case CycleType::NametableByte1:
      return os << "NametableByte1";
    case CycleType::AttrByte0:
      return os << "AttrByte0";
    case CycleType::AttrByte1:
      return os << "AttrByte1";
    case CycleType::PatternTileLow0:
      return os << "PatternTileLow0";
    case CycleType::PatternTileLow1:
      return os << "PatternTileLow1";
    case CycleType::PatternTileHigh0:
      return os << "PatternTileHigh0";
    case CycleType::PatternTileHigh1:
      return os << "PatternTileHigh1";
    case CycleType::GarbageByte0:
      return os << "GarbageByte0";
    case CycleType::GarbageByte1:
      return os << "GarbageByte1";
    case CycleType::GarbageByte2:
      return os << "GarbageByte2";
    case CycleType::GarbageByte3:
      return os << "GarbageByte3";
    case CycleType::NextSpriteTileLow0:
      return os << "NextSpriteTileLow0";
    case CycleType::NextSpriteTileLow1:
      return os << "NextSpriteTileLow1";
    case CycleType::NextSpriteTileHigh0:
      return os << "NextSpriteTileHigh0";
    case CycleType::NextSpriteTileHigh1:
      return os << "NextSpriteTileHigh1";
    case CycleType::NextNametableByte0:
      return os << "NextNametableByte0";
    case CycleType::NextNametableByte1:
      return os << "NextNametableByte1";
    case CycleType::NextAttrByte0:
      return os << "NextAttrByte0";
    case CycleType::NextAttrByte1:
      return os << "NextAttrByte1";
    case CycleType::NextPatternTileLow0:
      return os << "NextPatternTileLow0";
    case CycleType::NextPatternTileLow1:
      return os << "NextPatternTileLow1";
    case CycleType::NextPatternTileHigh0:
      return os << "NextPatternTileHigh0";
    case CycleType::NextPatternTileHigh1:
      return os << "NextPatternTileHigh1";
    case CycleType::FirstUnkByte0:
      return os << "FirstUnkByte0";
    case CycleType::FirstUnkByte1:
      return os << "FirstUnkByte1";
    case CycleType::SecondUnkByte0:
      return os << "SecondUnkByte0";
    case CycleType::SecondUnkByte1:
      return os << "SecondUnkByte1";
  }
}

// std::ostream& operator<<(std::ostream& os,
//                          const SpriteFetchType& sprite_fetch_type) {
//   switch (sprite_fetch_type) {
//     case SpriteFetchType::SecondaryOamInit:
//       return os << "SecondaryOamInit";
//     case SpriteFetchType::SpriteEval:
//       return os << "SpriteEval";
//     case SpriteFetchType::SpriteFetch:
//       return os << "SpriteFetch";
//     case SpriteFetchType::RenderPipelineInit:
//       return os << "RenderPipelineInit";
//   }
// }

}  // namespace graphics
