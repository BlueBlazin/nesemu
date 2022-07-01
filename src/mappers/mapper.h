#ifndef SRC_MAPPERS_MAPPER_H_
#define SRC_MAPPERS_MAPPER_H_

#include <cstdint>

namespace mappers {

class Mapper {
 public:
  virtual uint8_t CpuRead(uint16_t addr) = 0;
  virtual void CpuWrite(uint16_t addr, uint8_t value) = 0;
  virtual uint8_t PpuRead(uint16_t addr) = 0;
  virtual void PpuWrite(uint16_t addr, uint8_t value) = 0;
  virtual ~Mapper() {}
};

}  // namespace mappers

#endif  // SRC_MAPPERS_MAPPER_H_