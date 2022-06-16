#ifndef SRC_MAPPERS_INES_
#define SRC_MAPPERS_INES_

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "src/mappers/mapper.h"
#include "src/ppu/ppu.h"

namespace mappers {

constexpr uint64_t INES_HEADER_SIZE = 16;
constexpr uint64_t TRAINER_SIZE = 512;

struct INesHeader {
  uint64_t prg_rom_size;
  uint64_t chr_rom_size;
  // Flags 6
  graphics::Mirroring mirroring;
  bool has_battery;
  bool has_trainer;
};

std::shared_ptr<Mapper> ReadCartridge(std::string path);

}  // namespace mappers

#endif  // SRC_MAPPERS_INES_