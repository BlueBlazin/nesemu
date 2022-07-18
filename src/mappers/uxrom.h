#ifndef SRC_MAPPERS_UXROM_H_
#define SRC_MAPPERS_UXROM_H_

#include <array>
#include <cstdint>
#include <vector>

#include "src/mappers/ines.h"
#include "src/mappers/mapper.h"
#include "src/mirroring/mirroring.h"

namespace mappers {

namespace {

constexpr int BANK_SIZE = 16 * 1024;

}  // namespace

class UxRom : public Mapper {
 public:
  UxRom(INesHeader header, std::vector<uint8_t> data);

  uint8_t CpuRead(uint16_t addr) override;
  void CpuWrite(uint16_t addr, uint8_t value) override;
  uint8_t PpuRead(uint16_t addr) override;
  void PpuWrite(uint16_t addr, uint8_t value) override;

 private:
  uint8_t VramRead(uint16_t addr);
  void VramWrite(uint16_t addr, uint8_t value);

  std::vector<uint8_t> prg_rom;
  std::vector<uint8_t> chr_rxm;
  std::array<uint8_t, 4096> vram;
  graphics::Mirroring mirroring;
  int num_banks;
  uint16_t bank = 0;
};

}  // namespace mappers

#endif  // SRC_MAPPERS_UXROM_H_