#ifndef SRC_MEMORY_MEMORY_H_
#define SRC_MEMORY_MEMORY_H_

#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include "src/mappers/ines.h"
#include "src/mappers/mapper.h"
#include "src/ppu/ppu.h"

namespace memory {

enum class DmaState {
  Read,
  Write,
};

class Memory {
 public:
  Memory(const std::string& path, uint8_t& p1_input);

  void PpuTick(uint64_t n);
  void DmaTick();

  uint8_t* GetScreen();
  uint8_t* GetPatTable1();
  uint8_t* GetPatTable2();
  uint8_t* GetNametable(uint16_t addr);
  uint8_t* GetSprites();
  uint8_t* GetPalettes();

  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);
  bool NmiPending();
  void ClearNmi();
  bool InDma();

 private:
  std::shared_ptr<mappers::Mapper> cartridge;
  graphics::Ppu ppu;
  std::array<uint8_t, 0x800> ram;
  bool in_dma = false;
  uint8_t dma_data = 0x00;
  uint16_t dma_addr = 0x0000;
  DmaState dma_state = DmaState::Read;

  // controller
  uint8_t& p1_input;
  bool strobe = false;
  uint8_t p1_data = 0x00;
};

}  // namespace memory

#endif  // SRC_MEMORY_MEMORY_H_
