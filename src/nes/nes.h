#ifndef SRC_NES_NES_H_
#define SRC_NES_NES_H_

#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "src/cpu/cpu.h"
#include "src/cpu/event.h"
#include "src/mappers/mapper.h"

namespace nes {

constexpr int SCREEN_WIDTH = 256;
constexpr int SCREEN_HEIGHT = 240;
constexpr int PAT_TABLE_SIZE = 128;
constexpr int SPRITES_WIDTH = 64;
constexpr int SPRITES_HEIGHT = 128;
// rough estimate of title bar height
constexpr int TITLEBAR_HEIGHT = 65;
constexpr uint64_t MAX_CYCLES = 29815;
constexpr float TIME_PER_FRAME = 1.0 / 60.0;

class Nes {
 public:
  Nes(const std::string& rom_path);

  void Run();

 private:
  void Emulate();
  void HandleEvents();
  void UpdateWindows();
  void InitialDraw();
  void DrawWindows();
  void DisplayWindows();

  cpu::Cpu cpu;

  sf::RenderWindow window;
  // debug windows
  sf::RenderWindow pat_table1_window;
  sf::RenderWindow pat_table2_window;

  sf::RenderWindow nametable1_window;
  sf::RenderWindow nametable2_window;
  sf::RenderWindow nametable3_window;
  sf::RenderWindow nametable4_window;

  // objects = game sprites
  sf::RenderWindow objects_window;

  // textures
  sf::Texture texture;

  sf::Texture pt1_texture;
  sf::Texture pt2_texture;

  sf::Texture nt1_texture;
  sf::Texture nt2_texture;
  sf::Texture nt3_texture;
  sf::Texture nt4_texture;

  sf::Texture objects_texture;

  // sprites
  sf::Sprite window_sprite;
  sf::Sprite pt1_sprite;
  sf::Sprite pt2_sprite;
  sf::Sprite nt1_sprite;
  sf::Sprite nt2_sprite;
  sf::Sprite nt3_sprite;
  sf::Sprite nt4_sprite;
  sf::Sprite objects_sprite;

  // events
  sf::Event event;

  // internal
  bool cmd_pressed = false;
  std::unordered_map<int, int> key_offsets;
};

}  // namespace nes

#endif  // SRC_NES_NES_H_