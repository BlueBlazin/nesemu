#ifndef SRC_MIRRORING_MIRRORING_H_
#define SRC_MIRRORING_MIRRORING_H_

namespace graphics {

enum class Mirroring {
  Horizontal,
  Vertical,
  FourScreen,
};

inline uint16_t horizontal_mirrored(uint16_t addr) {
  if (addr < 0x2400) {
    return addr - 0x2000 + 0x0000;
  } else if (addr < 0x2800) {
    return addr - 0x2400 + 0x0000;
  } else if (addr < 0x2C00) {
    return addr - 0x2800 + 0x0400;
  } else {
    return addr - 0x2C00 + 0x0400;
  }
}

inline uint16_t vertical_mirrored(uint16_t addr) {
  if (addr < 0x2400) {
    return addr - 0x2000 + 0x0000;
  } else if (addr < 0x2800) {
    return addr - 0x2400 + 0x0400;
  } else if (addr < 0x2C00) {
    return addr - 0x2800 + 0x0000;
  } else {
    return addr - 0x2C00 + 0x0400;
  }
}

}  // namespace graphics

#endif  // SRC_MIRRORING_MIRRORING_H_