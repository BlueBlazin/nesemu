#include "nes.h"

#include <cmath>
#include <cstdint>
#include <string>

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "src/controllers/controllers.h"
#include "src/cpu/cpu.h"
#include "src/cpu/event.h"
#include "src/mappers/mapper.h"

namespace nes {

Nes::Nes(const std::string& rom_path)
    : cpu(rom_path),
      window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "NESEmu"),
      pat_table1_window(sf::VideoMode(PAT_TABLE_SIZE, PAT_TABLE_SIZE),
                        "Pattern Table 1",
                        sf::Style::Titlebar | sf::Style::Resize),
      pat_table2_window(sf::VideoMode(PAT_TABLE_SIZE, PAT_TABLE_SIZE),
                        "Pattern Table 2",
                        sf::Style::Titlebar | sf::Style::Resize),
      nametable1_window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
                        "Nametable at 0x2000",
                        sf::Style::Titlebar | sf::Style::Resize),
      nametable2_window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
                        "Nametable at 0x2400",
                        sf::Style::Titlebar | sf::Style::Resize),
      nametable3_window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
                        "Nametable at 0x2800",
                        sf::Style::Titlebar | sf::Style::Resize),
      nametable4_window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT),
                        "Nametable at 0x2C00",
                        sf::Style::Titlebar | sf::Style::Resize) {
  // get screen dimensions
  uint64_t width = sf::VideoMode::getDesktopMode().width;
  uint64_t height = sf::VideoMode::getDesktopMode().height;

  // calculate zoom with a base screen width of 1024
  int zoom = static_cast<int>(std::ceil(static_cast<double>(width) / 1024.0));

  // window resize dimensions
  int screen_width = SCREEN_WIDTH * zoom;
  int screen_height = SCREEN_WIDTH * zoom;
  int pat_table_size = PAT_TABLE_SIZE * (zoom + 1);
  int nametable_width = SCREEN_WIDTH * 2;
  int nametable_height = SCREEN_HEIGHT * 2;
  // 50px padding
  int padding = 50;

  // resize windows
  window.setSize(sf::Vector2u(screen_width, screen_height));
  pat_table1_window.setSize(sf::Vector2u(pat_table_size, pat_table_size));
  pat_table2_window.setSize(sf::Vector2u(pat_table_size, pat_table_size));
  nametable1_window.setSize(sf::Vector2u(nametable_width, nametable_height));
  nametable2_window.setSize(sf::Vector2u(nametable_width, nametable_height));
  nametable3_window.setSize(sf::Vector2u(nametable_width, nametable_height));
  nametable4_window.setSize(sf::Vector2u(nametable_width, nametable_height));

  // compute aggregate dimensions
  int total_windows_width =
      pat_table_size + screen_width + 2 * nametable_width + 3 * padding;

  int pat_tables_height = 2 * pat_table_size + padding + TITLEBAR_HEIGHT;
  int nametables_height = 2 * nametable_height + padding + TITLEBAR_HEIGHT;

  // position pattern tables
  int pt_x = (width - total_windows_width) / 2;
  int pt_y = (height - pat_tables_height) / 2;
  pat_table1_window.setPosition(sf::Vector2i(pt_x, pt_y));
  pat_table2_window.setPosition(
      sf::Vector2i(pt_x, pt_y + pat_table_size + padding + TITLEBAR_HEIGHT));

  // position main window
  int window_x = pt_x + pat_table_size + padding;
  int window_y = (height - screen_height) / 2;
  window.setPosition(sf::Vector2i(window_x, window_y));

  // position nametables
  int nt_x = window_x + screen_width + padding;
  int nt_y = (height - nametables_height) / 2;
  nametable1_window.setPosition(sf::Vector2i(nt_x, nt_y));
  nametable2_window.setPosition(
      sf::Vector2i(nt_x, nt_y + nametable_height + padding + TITLEBAR_HEIGHT));
  nametable3_window.setPosition(
      sf::Vector2i(nt_x + nametable_width + padding, nt_y));
  nametable4_window.setPosition(
      sf::Vector2i(nt_x + nametable_width + padding,
                   nt_y + nametable_height + padding + TITLEBAR_HEIGHT));

  // create textures
  texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);

  pt1_texture.create(PAT_TABLE_SIZE, PAT_TABLE_SIZE);
  pt2_texture.create(PAT_TABLE_SIZE, PAT_TABLE_SIZE);

  nt1_texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
  nt2_texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
  nt3_texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
  nt4_texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);

  // define sprites
  // TODO: can this be done in initializer list instead?
  window_sprite = sf::Sprite(texture);
  pt1_sprite = sf::Sprite(pt1_texture);
  pt2_sprite = sf::Sprite(pt2_texture);
  nt1_sprite = sf::Sprite(nt1_texture);
  nt2_sprite = sf::Sprite(nt2_texture);
  nt3_sprite = sf::Sprite(nt3_texture);
  nt4_sprite = sf::Sprite(nt4_texture);
}

void Nes::Run() {
  // start cpu
  cpu.Startup();
  // display windows
  InitialDraw();

  // define clock
  sf::Clock clock;
  // time delta
  float dt = 0.0F;
  // total elapsed time
  float elapsed = 0.0F;

  while (window.isOpen()) {
    // handle events
    HandleEvents();

    // run emulation forward
    if ((elapsed = clock.getElapsedTime().asSeconds() + dt) >= TIME_PER_FRAME) {
      dt = elapsed - TIME_PER_FRAME;
      clock.restart();
      Emulate();
    }
  }
}

void Nes::Emulate() {
  while (true) {
    switch (cpu.RunTillEvent(MAX_CYCLES)) {
      case cpu::Event::VBlank:
        UpdateWindows();
        break;
      case cpu::Event::MaxCycles:
        return;
    }
  }
}

void Nes::HandleEvents() {
  if (!window.pollEvent(event)) {
    return;
  }

  if (event.type == sf::Event::Closed) {
    window.close();
  } else if (event.type == sf::Event::KeyPressed) {
    switch (event.key.code) {
      case sf::Keyboard::A:
        cpu.PressKey(controllers::Key::A);
        break;
      case sf::Keyboard::S:
        cpu.PressKey(controllers::Key::B);
        break;
      case sf::Keyboard::Space:
        cpu.PressKey(controllers::Key::Select);
        break;
      case sf::Keyboard::Enter:
        cpu.PressKey(controllers::Key::Start);
        break;
      case sf::Keyboard::Up:
        cpu.PressKey(controllers::Key::Up);
        break;
      case sf::Keyboard::Down:
        cpu.PressKey(controllers::Key::Down);
        break;
      case sf::Keyboard::Left:
        cpu.PressKey(controllers::Key::Left);
        break;
      case sf::Keyboard::Right:
        cpu.PressKey(controllers::Key::Right);
        break;
      case sf::Keyboard::LSystem:
      case sf::Keyboard::RSystem:
        cmd_pressed = true;
        break;
      case sf::Keyboard::Q:
      case sf::Keyboard::W:
        window.close();
        break;
    }
  } else if (event.type == sf::Event::KeyReleased) {
    switch (event.key.code) {
      case sf::Keyboard::A:
        cpu.ReleaseKey(controllers::Key::A);
        break;
      case sf::Keyboard::S:
        cpu.ReleaseKey(controllers::Key::B);
        break;
      case sf::Keyboard::Space:
        cpu.ReleaseKey(controllers::Key::Select);
        break;
      case sf::Keyboard::Enter:
        cpu.ReleaseKey(controllers::Key::Start);
        break;
      case sf::Keyboard::Up:
        cpu.ReleaseKey(controllers::Key::Up);
        break;
      case sf::Keyboard::Down:
        cpu.ReleaseKey(controllers::Key::Down);
        break;
      case sf::Keyboard::Left:
        cpu.ReleaseKey(controllers::Key::Left);
        break;
      case sf::Keyboard::Right:
        cpu.ReleaseKey(controllers::Key::Right);
        break;
      case sf::Keyboard::LSystem:
      case sf::Keyboard::RSystem:
        cmd_pressed = false;
        break;
    }
  }
}

void Nes::UpdateWindows() {
  texture.update(cpu.GetScreen());
  pt1_texture.update(cpu.GetPatTable1());
  pt2_texture.update(cpu.GetPatTable2());
  nt1_texture.update(cpu.GetNametable(0x2000));
  nt2_texture.update(cpu.GetNametable(0x2400));
  nt3_texture.update(cpu.GetNametable(0x2800));
  nt4_texture.update(cpu.GetNametable(0x2C00));

  DrawWindows();
  DisplayWindows();
}

void Nes::InitialDraw() {
  DrawWindows();
  DisplayWindows();
}

void Nes::DrawWindows() {
  window.draw(window_sprite);
  pat_table1_window.draw(pt1_sprite);
  pat_table2_window.draw(pt2_sprite);
  nametable1_window.draw(nt1_sprite);
  nametable2_window.draw(nt2_sprite);
  nametable3_window.draw(nt3_sprite);
  nametable4_window.draw(nt4_sprite);
}

void Nes::DisplayWindows() {
  window.display();

  pat_table1_window.display();
  pat_table2_window.display();

  nametable1_window.display();
  nametable2_window.display();
  nametable3_window.display();
  nametable4_window.display();
}

}  // namespace nes