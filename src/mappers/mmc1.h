#ifndef SRC_MAPPERS_MMC1_H_
#define SRC_MAPPERS_MMC1_H_

#include <array>
#include <cstdint>
#include <vector>

#include "src/mappers/ines.h"
#include "src/mappers/mapper.h"
#include "src/mirroring/mirroring.h"

namespace mappers {

class Mmc1 : public Mapper {
 public:
  Mmc1(INesHeader header, std::vector<uint8_t> data);

  uint8_t CpuRead(uint16_t addr) override;
  void CpuWrite(uint16_t addr, uint8_t value) override;
  uint8_t PpuRead(uint16_t addr) override;
  void PpuWrite(uint16_t addr, uint8_t value) override;

 private:
  std::vector<uint8_t> prg_ram;
  std::vector<uint8_t> prg_rom;
};

// class Nrom : public Mapper {
//  public:
//   Nrom(INesHeader header, std::vector<uint8_t> data);

//   uint8_t CpuRead(uint16_t addr) override;
//   void CpuWrite(uint16_t addr, uint8_t value) override;
//   uint8_t PpuRead(uint16_t addr) override;
//   void PpuWrite(uint16_t addr, uint8_t value) override;

//  private:
//   uint8_t VramRead(uint16_t addr);
//   void VramWrite(uint16_t addr, uint8_t value);

//   bool nrom256;
//   std::vector<uint8_t> prg_rom;
//   std::array<uint8_t, 8192> prg_ram;
//   std::vector<uint8_t> chr_rxm;
//   std::array<uint8_t, 4096> vram;
//   graphics::Mirroring mirroring;
//   bool flag = false;
// };

}  // namespace mappers

#endif  // SRC_MAPPERS_MMC1_H_