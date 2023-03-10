#include "src/mappers/ines.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <vector>

#include "src/mappers/nrom.h"
#include "src/mappers/uxrom.h"
#include "src/mirroring/mirroring.h"

namespace mappers {

std::shared_ptr<Mapper> ReadCartridge(std::string path) {
  std::ifstream stream(path, std::ios::in | std::ios::binary);
  std::istreambuf_iterator<char> start(stream), end;
  std::vector<uint8_t> data(start, end);

  uint8_t flag6 = data[6];

  graphics::Mirroring mirroring;

  if (static_cast<bool>(flag6 & 0x8)) {
    mirroring = graphics::Mirroring::FourScreen;
  } else {
    mirroring = static_cast<bool>(flag6 & 0x1)
                    ? graphics::Mirroring::Vertical
                    : graphics::Mirroring::Horizontal;
  }

  INesHeader header = {
      .prg_rom_size = static_cast<uint64_t>(
          std::max(static_cast<uint8_t>(1), data[4]) * 16 * 1024),
      .chr_rom_size = static_cast<uint64_t>(
          std::max(static_cast<uint8_t>(1), data[5]) * 8 * 1024),
      .mirroring = mirroring,
      .has_battery = static_cast<bool>(flag6 & 0x2),
      .has_trainer = static_cast<bool>(flag6 & 4),
  };

  std::cout << "flag6: " << ((flag6 >> 4) & 0xF) << std::endl;

  switch ((flag6 >> 4) & 0xF) {
    case 0:
      return std::make_shared<Nrom>(header, std::move(data));
    case 2:
      return std::make_unique<UxRom>(header, std::move(data));
  }

  return nullptr;
}

}  // namespace mappers
