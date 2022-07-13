#include <cstdint>
#include <cstdio>
#include <iostream>

#include "ppu.h"
#include "src/ppu/palette.h"

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
  uint16_t attr_base_addr = addr | 0x03C0;

  for (int byte = 0; byte < NAMETABLE_ROWS * NAMETABLE_COLS; byte++) {
    // pattern table index
    uint8_t offset = ReadVram(addr + static_cast<uint16_t>(byte));
    uint16_t pattern_addr = offset * 16;

    // nametable row & col
    int nt_row = byte / NAMETABLE_COLS;
    int nt_col = byte % NAMETABLE_COLS;

    // attribute table row & col
    int at_row = nt_row / 4;
    int at_col = nt_col / 4;
    // attribute table offset
    uint16_t attr_offset = at_row * 8 + at_col;
    // attribute value address
    uint16_t attr_addr = attr_base_addr + attr_offset;
    // attribute value
    uint8_t attr_value = ReadVram(attr_addr);

    // attribute table quadrant row & col
    int qd_row = (nt_row / 2) % 2;
    int qd_col = (nt_col / 2) % 2;
    // attribute value shift
    int shift = qd_row * 2 + qd_col;

    // palette
    // uint8_t palette = (attr_value >> shift) & 0x3;
    uint8_t palette = attr_value & 0x3;

    for (int i = 0; i < 8; i++) {
      uint8_t row_lo = ReadVram(table_offset + pattern_addr + i);
      uint8_t row_hi = ReadVram(table_offset + pattern_addr + i + 8);

      for (int j = 0; j < 8; j++) {
        uint8_t col_lo = (row_lo >> (7 - j)) & 0x1;
        uint8_t col_hi = (row_hi >> (7 - j)) & 0x1;
        uint8_t value = (col_hi << 1) | col_lo;  // 85 == 255 / 3
        Color color = GetRgb(value == 0 ? 0 : palette, value, 0x0);

        int coarse_x = 8 * (byte % NAMETABLE_COLS);
        int coarse_y = 8 * (byte / NAMETABLE_COLS);

        int x = coarse_x + j;
        int y = coarse_y + i;

        int idx = (y * SCREEN_WIDTH + x) * SCREEN_CHANNELS;

        switch (addr) {
          case 0x2000: {
            nametable1[idx + 0] = color.red;
            nametable1[idx + 1] = color.green;
            nametable1[idx + 2] = color.blue;
            nametable1[idx + 3] = 0xFF;
          }
          case 0x2400: {
            nametable2[idx + 0] = color.red;
            nametable2[idx + 1] = color.green;
            nametable2[idx + 2] = color.blue;
            nametable2[idx + 3] = 0xFF;
          }
          case 0x2800: {
            nametable3[idx + 0] = color.red;
            nametable3[idx + 1] = color.green;
            nametable3[idx + 2] = color.blue;
            nametable3[idx + 3] = 0xFF;
          }
          case 0x2C00: {
            nametable4[idx + 0] = color.red;
            nametable4[idx + 1] = color.green;
            nametable4[idx + 2] = color.blue;
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
      addr = sprite_table_addr | (tile_idx << 4);
    }

    int x = (n % SPRITES_COLS) * SPRITE_BOX_WIDTH;
    int y = (n / SPRITES_COLS) * SPRITE_BOX_HEIGHT;
    // draw 8x8 sprite
    for (int row = y + 2, i = 0; row < y + 2 + 8; row++, i++) {
      uint8_t lo = ReadVram(addr + i);
      uint8_t hi = ReadVram(addr + i + 8);

      for (int col = x + 2; col < x + 2 + 8; col++) {
        uint8_t value = ((hi >> 6) & 0x2) | (lo >> 7);
        Color color = GetRgb(value == 0 ? 0 : palette, value, 0x10);

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

  // draw white border around sprites
  for (int i = 0; i < SPRITES_HEIGHT; i++) {
    for (int j = 0; j < SPRITES_WIDTH; j++) {
      int idx = (i * SPRITES_WIDTH + j) * SCREEN_CHANNELS;

      if ((i % SPRITE_BOX_HEIGHT == 1 || i % SPRITE_BOX_HEIGHT == 18) &&
          !(j % SPRITE_BOX_WIDTH == 0 || j % SPRITE_BOX_WIDTH == 11)) {
        sprites[idx + 0] = 0xDD;
        sprites[idx + 1] = 0xDD;
        sprites[idx + 2] = 0xDD;
        sprites[idx + 3] = 0xDD;
      } else if ((j % SPRITE_BOX_WIDTH == 1 || j % SPRITE_BOX_WIDTH == 10) &&
                 !(i % SPRITE_BOX_HEIGHT == 0 || i % SPRITE_BOX_HEIGHT == 19)) {
        sprites[idx + 0] = 0xDD;
        sprites[idx + 1] = 0xDD;
        sprites[idx + 2] = 0xDD;
        sprites[idx + 3] = 0xDD;
      }
    }
  }
}

void Ppu::UpdatePalettes() {
  constexpr int rect_height = PALETTE_PADDING + PALETTES_BOX_HEIGHT;
  constexpr int rect_width = PALETTE_PADDING + PALETTES_BOX_WIDTH;

  for (int i = 0; i < PALETTES_HEIGHT; i++) {
    // skip padding rows
    if (i % rect_height < PALETTE_PADDING) {
      continue;
    }

    for (int j = 0; j < PALETTES_WIDTH; j++) {
      int idx = (i * PALETTES_WIDTH + j) * SCREEN_CHANNELS;

      // skip padding columns
      if (j % rect_width < PALETTE_PADDING) {
        continue;
      }
      // border columns
      int slot = j / rect_width;

      if ((i % rect_height == PALETTE_PADDING) ||
          (i % rect_height == rect_height - 1)) {
        palettes[idx + 0] = 0xFF;
        palettes[idx + 1] = 0xFF;
        palettes[idx + 2] = 0xFF;
        palettes[idx + 3] = 0xFF;
        continue;
      }

      if ((j - (PALETTE_PADDING + rect_width * slot)) % PALETTE_SIZE == 0) {
        palettes[idx + 0] = 0xFF;
        palettes[idx + 1] = 0xFF;
        palettes[idx + 2] = 0xFF;
        palettes[idx + 3] = 0xFF;
        continue;
      }

      int n = (j - (PALETTE_PADDING + rect_width * slot)) / PALETTE_SIZE;
      uint16_t offset = i < rect_height ? 0 : 0x10;
      uint16_t master_palette_idx =
          static_cast<uint16_t>(ReadVram(PALETTE_ADDRS[slot][n] | offset)) * 3;
      palettes[idx + 0] = NTSC_PALETTE[master_palette_idx + 0];
      palettes[idx + 1] = NTSC_PALETTE[master_palette_idx + 1];
      palettes[idx + 2] = NTSC_PALETTE[master_palette_idx + 2];
      palettes[idx + 3] = 0xFF;
    }
  }
}

}  // namespace graphics