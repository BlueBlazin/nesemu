#include <bitset>
#include <iostream>

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "src/cpu/cpu.h"

// int main() {
//   // MAIN
//   return EXIT_SUCCESS;
// }

int main() {
  // Window
  sf::RenderWindow window(sf::VideoMode(256 * 4, 240 * 4), "NESEmu",
                          sf::Style::Titlebar | sf::Style::Close);
  sf::Event ev;

  // Game loop
  while (window.isOpen()) {
    while (window.pollEvent(ev)) {
      switch (ev.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          if (ev.key.code == sf::Keyboard::Escape) {
            window.close();
          }
          break;
      }
    }

    window.clear(sf::Color::Blue);
    window.display();
  }

  return 0;
}