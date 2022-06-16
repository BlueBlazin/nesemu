#ifndef SRC_MAPPERS_NROM_
#define SRC_MAPPERS_NROM_

#include <array>
#include <cstdint>
#include <vector>

#include "src/mappers/ines.h"
#include "src/mappers/mapper.h"
#include "src/ppu/ppu.h"

namespace mappers {

class Nrom : public Mapper {
 public:
  Nrom(INesHeader header, std::vector<uint8_t> data);

  uint8_t CpuRead(uint16_t addr) override;
  void CpuWrite(uint16_t addr, uint8_t value) override;
  uint8_t PpuRead(uint16_t addr) override;
  void PpuWrite(uint16_t addr, uint8_t value) override;

 private:
  uint8_t VramRead(uint16_t addr);
  void VramWrite(uint16_t addr, uint8_t value);

  bool nrom256;
  std::vector<uint8_t> prg_rom;
  std::array<uint8_t, 8192> prg_ram;
  std::vector<uint8_t> chr_rxm;
  std::array<uint8_t, 4096> vram;
  graphics::Mirroring mirroring;
};

}  // namespace mappers

#endif  // SRC_MAPPERS_NROM_