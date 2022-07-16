#ifndef SRC_MAPPERS_UXROM_H_
#define SRC_MAPPERS_UXROM_H_

#include <array>
#include <cstdint>
#include <vector>

#include "src/mappers/ines.h"
#include "src/mappers/mapper.h"
#include "src/mirroring/mirroring.h"

namespace mappers {

class UxRom : public Mapper {
 public:
  UxRom(INesHeader header, std::vector<uint8_t> data);

  uint8_t CpuRead(uint16_t addr) override;
  void CpuWrite(uint16_t addr, uint8_t value) override;
  uint8_t PpuRead(uint16_t addr) override;
  void PpuWrite(uint16_t addr, uint8_t value) override;

 private:
  std::vector<uint8_t> prg_ram;
  std::vector<uint8_t> prg_rom;
};

}  // namespace mappers

#endif  // SRC_MAPPERS_UXROM_H_