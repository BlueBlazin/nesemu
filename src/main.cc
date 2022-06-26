#include <bitset>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "src/cpu/cpu.h"
#include "src/mappers/mapper.h"
#include "src/mappers/nrom.h"

int main(int argc, char *argv[]) {
  cpu::Cpu cpu = cpu::Cpu(argv[1]);
  cpu.Startup();

  sf::RenderWindow window(sf::VideoMode(256, 240), "NESEmu");
  // window.setSize(sf::Vector2u(1024, 960));
  window.setSize(sf::Vector2u(256 * 2, 240 * 2));
  window.setPosition(sf::Vector2i(1250, 400));

  sf::RenderWindow pat_table1_window(sf::VideoMode(128, 128),
                                     "Pattern Table 1");
  pat_table1_window.setPosition(sf::Vector2i(500, 600));
  pat_table1_window.setSize(sf::Vector2u(640, 640));

  sf::RenderWindow pat_table2_window(sf::VideoMode(128, 128),
                                     "Pattern Table 2");
  pat_table2_window.setPosition(sf::Vector2i(500, 1400));
  pat_table2_window.setSize(sf::Vector2u(640, 640));

  sf::RenderWindow nametable_window(sf::VideoMode(256, 240), "Nametable");
  nametable_window.setPosition(sf::Vector2i(2350, 500));
  nametable_window.setSize(sf::Vector2u(512, 480));

  sf::Event ev;
  sf::Texture texture;
  texture.create(256, 240);

  sf::Texture pat_table1_texture;
  pat_table1_texture.create(128, 128);

  sf::Texture pat_table2_texture;
  pat_table2_texture.create(128, 128);

  sf::Texture nametable_texture;
  nametable_texture.create(256, 240);

  for (uint64_t i = 0; i < 100000; i++) {
    cpu.Tick();
  }

  texture.update(cpu.GetScreen());
  pat_table1_texture.update(cpu.GetPatTable1());
  pat_table2_texture.update(cpu.GetPatTable2());
  nametable_texture.update(cpu.GetNametable());

  window.draw(sf::Sprite(texture));
  pat_table1_window.draw(sf::Sprite(pat_table1_texture));
  pat_table2_window.draw(sf::Sprite(pat_table2_texture));
  nametable_window.draw(sf::Sprite(nametable_texture));

  window.display();
  pat_table1_window.display();
  pat_table2_window.display();
  nametable_window.display();

  while (window.isOpen()) {
    bool event_polled = window.pollEvent(ev);

    if (event_polled && ev.type == sf::Event::Closed) {
      window.close();
    }
  }

  return EXIT_SUCCESS;
}

// int main(int argc, char *argv[]) {
//   cpu::Cpu cpu = cpu::Cpu(argv[1]);
//   cpu.Startup();

//   sf::RenderWindow window(sf::VideoMode(256, 240), "NESEmu");
//   window.setSize(sf::Vector2u(256 * 2, 240 * 2));

//   sf::RenderWindow pat_table1_window(sf::VideoMode(128, 128),
//                                      "Pattern Table 1");
//   pat_table1_window.setPosition(sf::Vector2i(300, 500));
//   pat_table1_window.setSize(sf::Vector2u(640, 640));

//   sf::RenderWindow pat_table2_window(sf::VideoMode(128, 128),
//                                      "Pattern Table 2");
//   pat_table2_window.setPosition(sf::Vector2i(1000, 500));
//   pat_table2_window.setSize(sf::Vector2u(640, 640));

//   sf::RenderWindow nametable_window(sf::VideoMode(256, 240), "Nametable");
//   nametable_window.setPosition(sf::Vector2i(2350, 500));
//   nametable_window.setSize(sf::Vector2u(512, 480));

//   sf::Event ev;
//   sf::Texture texture;
//   texture.create(256, 240);

//   sf::Texture pat_table1_texture;
//   pat_table1_texture.create(128, 128);

//   sf::Texture pat_table2_texture;
//   pat_table2_texture.create(128, 128);

//   sf::Texture nametable_texture;
//   nametable_texture.create(256, 240);

//   uint64_t i = 0;

//   while (window.isOpen()) {
//     bool event_polled = window.pollEvent(ev);

//     if (event_polled && ev.type == sf::Event::Closed) {
//       window.close();
//       pat_table1_window.close();
//       pat_table2_window.close();
//     }

//     cpu.Tick();
//     texture.update(cpu.GetScreen());

//     if (i % 256 == 0) {
//       pat_table1_texture.update(cpu.GetPatTable1());
//       pat_table2_texture.update(cpu.GetPatTable2());
//       nametable_texture.update(cpu.GetNametable());
//     }

//     window.draw(sf::Sprite(texture));
//     pat_table1_window.draw(sf::Sprite(pat_table1_texture));
//     pat_table2_window.draw(sf::Sprite(pat_table2_texture));
//     nametable_window.draw(sf::Sprite(nametable_texture));

//     window.display();
//     pat_table1_window.display();
//     pat_table2_window.display();
//     nametable_window.display();

//     i++;
//   }

//   return EXIT_SUCCESS;
// }

// int main(int argc, char *argv[]) {
//   cpu::Cpu cpu = cpu::Cpu(argv[1]);
//   cpu.Startup();

//   sf::RenderWindow window(sf::VideoMode(256, 240), "NESEmu");

//   sf::Event ev;
//   sf::Texture texture;
//   texture.create(256, 240);

//   while (window.isOpen()) {
//     bool event_polled = window.pollEvent(ev);

//     if (event_polled && ev.type == sf::Event::Closed) {
//       window.close();
//     } else if (true || event_polled && ev.type == sf::Event::KeyPressed) {
//       cpu.Tick();
//     }

//     // window.clear();

//     texture.update(cpu.GetScreen());
//     sf::Sprite sprite(texture);
//     window.draw(sprite);

//     window.display();
//   }

//   return EXIT_SUCCESS;
// }

// int main() {
//   auto nrom = mappers::ReadCartridge("contra.nes");

//   // std::cout << static_cast<int>(nrom->CpuRead(0)) << std::endl;
//   // std::cout << static_cast<int>(nrom->PpuRead(0)) << std::endl;

//   return EXIT_SUCCESS;
// }

// int main() {
//   sf::RenderWindow window(sf::VideoMode(256, 240), "NESEmu");
//   uint8_t pixels[240][256][4];
//   memset(pixels, 0xFF, 256 * 240 * 4);

//   sf::Texture texture;
//   texture.create(256, 240);
//   texture.update(reinterpret_cast<uint8_t*>(pixels));

//   sf::Sprite sprite(texture);
//   sf::Event ev;

//   while (window.isOpen()) {
//     if (window.pollEvent(ev) && ev.type == sf::Event::Closed) {
//       window.close();
//     }

//     window.clear();
//     window.draw(sprite);
//     window.display();
//   }
// }

// int main() {
//   // MAIN
//   return EXIT_SUCCESS;
// }

// int main() {
//   // Window
//   sf::RenderWindow window(sf::VideoMode(256, 240), "NESEmu",
//                           sf::Style::Titlebar | sf::Style::Close);
//   sf::Event ev;

//   // Game loop
//   while (window.isOpen()) {
//     while (window.pollEvent(ev)) {
//       switch (ev.type) {
//         case sf::Event::Closed:
//           window.close();
//           break;
//         case sf::Event::KeyPressed:
//           if (ev.key.code == sf::Keyboard::Escape) {
//             window.close();
//           }
//           break;
//       }
//     }

//     window.clear(sf::Color::Blue);
//     window.display();
//   }

//   return 0;
// }