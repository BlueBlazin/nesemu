#include <cstdint>
#include <cstdio>
#include <iostream>

#include "ppu.h"

namespace graphics {

void Ppu::UpdatePatternTable(uint16_t table_offset) {
  auto& pat_table = table_offset == 0 ? pat_table1 : pat_table2;

  for (int tile = 0; tile < 256; tile++) {
    int offset = tile * 16;  // because 16 bytes form 1 tile

    for (int i = 0; i < 8; i++) {
      uint8_t row_lo = ReadVram(table_offset + offset + i);
      // the high byte for a row is offset by 8 bytes from the low byte
      uint8_t row_hi = ReadVram(table_offset + offset + i + 8);

      for (int j = 0; j < 8; j++) {
        uint8_t col_lo = (row_lo >> (7 - j)) & 0x1;
        uint8_t col_hi = (row_hi >> (7 - j)) & 0x1;
        uint8_t value = ((col_hi << 1) | col_lo) * 85;  // 85 == 255 / 3

        int coarse_x = (tile * 8) % PAT_TABLE_WIDTH;
        int coarse_y = 8 * ((tile * 8) / PAT_TABLE_WIDTH);

        int x = coarse_x + j;
        int y = coarse_y + i;

        int idx = (y * PAT_TABLE_WIDTH + x) * SCREEN_CHANNELS;
        pat_table[idx + 0] = value;
        pat_table[idx + 1] = value;
        pat_table[idx + 2] = value;
        pat_table[idx + 3] = 0xFF;
      }
    }
  }
}

void Ppu::UpdateNametable(uint16_t addr) {
  // pattern table at 0x0000 or 0x1000
  uint16_t table_offset = pattern_table_addr == 0 ? 0x0000 : 0x1000;

  for (int byte = 0; byte < NAMETABLE_ROWS * NAMETABLE_COLS; byte++) {
    // pattern table index
    uint8_t offset = ReadVram(addr + static_cast<uint16_t>(byte));
    uint16_t pattern_addr = offset * 16;

    for (int i = 0; i < 8; i++) {
      uint8_t row_lo = ReadVram(table_offset + pattern_addr + i);
      uint8_t row_hi = ReadVram(table_offset + pattern_addr + i + 8);

      for (int j = 0; j < 8; j++) {
        uint8_t col_lo = (row_lo >> (7 - j)) & 0x1;
        uint8_t col_hi = (row_hi >> (7 - j)) & 0x1;
        uint8_t value = ((col_hi << 1) | col_lo) * 85;  // 85 == 255 / 3

        int coarse_x = 8 * (byte % NAMETABLE_COLS);
        int coarse_y = 8 * (byte / NAMETABLE_COLS);

        int x = coarse_x + j;
        int y = coarse_y + i;

        int idx = (y * SCREEN_WIDTH + x) * SCREEN_CHANNELS;

        switch (addr) {
          case 0x2000: {
            nametable1[idx + 0] = value;
            nametable1[idx + 1] = value;
            nametable1[idx + 2] = value;
            nametable1[idx + 3] = 0xFF;
          }
          case 0x2400: {
            nametable2[idx + 0] = value;
            nametable2[idx + 1] = value;
            nametable2[idx + 2] = value;
            nametable2[idx + 3] = 0xFF;
          }
          case 0x2800: {
            nametable3[idx + 0] = value;
            nametable3[idx + 1] = value;
            nametable3[idx + 2] = value;
            nametable3[idx + 3] = 0xFF;
          }
          case 0x2C00: {
            nametable4[idx + 0] = value;
            nametable4[idx + 1] = value;
            nametable4[idx + 2] = value;
            nametable4[idx + 3] = 0xFF;
          }
        }
      }
    }
  }
}

void Ppu::UpdateSprites() {
  for (int n = 0; n < 64; n++) {
    uint16_t addr;
    uint8_t palette = obj_attr_memory[n << 2 | 2] & 0x3;

    if (long_sprites) {
      // TODO
    } else {
      uint8_t tile_idx = obj_attr_memory[n << 2 | 1];
      // std::printf("n: %d, 0x%X, 0x%X, 0x%X, 0x%X\n", n, obj_attr_memory[n *
      // 4],
      //             obj_attr_memory[n * 4 + 1], obj_attr_memory[n * 4 + 2],
      //             obj_attr_memory[n * 4 + 3]);
      addr = sprite_table_addr | (tile_idx << 4);
    }

    int x = (n % SPRITES_COLS) * 8;
    int y = (n / SPRITES_COLS) * 16;
    // draw 8x8 sprite
    for (int row = y; row < y + 8; row++) {
      uint8_t lo = ReadVram(addr + row);
      uint8_t hi = ReadVram(addr + row + 8);

      for (int col = x; col < x + 8; col++) {
        uint8_t value = ((hi >> 6) & 0x2) | (lo >> 7);
        Color color = GetRgb(palette, value, 0x10);

        int idx = (row * SPRITES_WIDTH + col) * SCREEN_CHANNELS;
        sprites[idx + 0] = color.red;
        sprites[idx + 1] = color.green;
        sprites[idx + 2] = color.blue;
        sprites[idx + 3] = 0xFF;

        lo = lo << 1;
        hi = hi << 1;
      }
    }
  }
}

}  // namespace graphics