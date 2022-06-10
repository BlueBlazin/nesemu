#include <bitset>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "src/cpu/cpu.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(256, 240), "NESEmu");
  uint8_t pixels[240][256][4];
  memset(pixels, 0xFF, 256 * 240 * 4);

  sf::Texture texture;
  texture.create(256, 240);
  texture.update(reinterpret_cast<uint8_t*>(pixels));

  sf::Sprite sprite(texture);
  sf::Event ev;

  while (window.isOpen()) {
    if (window.pollEvent(ev) && ev.type == sf::Event::Closed) {
      window.close();
    }

    window.clear();
    window.draw(sprite);
    window.display();
  }
}

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