#include "nes.h"

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "SFML/Audio.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
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
                        sf::Style::Titlebar | sf::Style::Resize),
      objects_window(sf::VideoMode(SPRITES_WIDTH, SPRITES_HEIGHT), "Sprites",
                     sf::Style::Titlebar | sf::Style::Resize),
      palettes_window(sf::VideoMode(PALETTES_WIDTH, PALETTES_HEIGHT),
                      "Palettes", sf::Style::Titlebar | sf::Style::Resize),
      key_offsets{
          {sf::Keyboard::A, 0},     {sf::Keyboard::S, 1},
          {sf::Keyboard::Space, 2}, {sf::Keyboard::Enter, 3},
          {sf::Keyboard::Up, 4},    {sf::Keyboard::Down, 5},
          {sf::Keyboard::Left, 6},  {sf::Keyboard::Right, 7},
      } {
  // get screen dimensions
  uint64_t width = sf::VideoMode::getDesktopMode().width;
  uint64_t height = sf::VideoMode::getDesktopMode().height;

  // calculate zoom with a base screen width of 1024
  int zoom = static_cast<int>(std::ceil(static_cast<double>(width) / 1024.0));

  // window resize dimensions
  int screen_width = SCREEN_WIDTH * zoom;
  int screen_height = SCREEN_WIDTH * zoom;
  // int pat_table_size = PAT_TABLE_SIZE * (zoom + 1);
  int pat_table_size = PAT_TABLE_SIZE * zoom;
  int nametable_width = SCREEN_WIDTH * 2;
  int nametable_height = SCREEN_HEIGHT * 2;
  int objects_width = SPRITES_WIDTH * (zoom + 1);
  int objects_height = SPRITES_HEIGHT * (zoom + 1);
  // int palettes_width = PALETTES_WIDTH * std::max(1, zoom - 1);
  // int palettes_height = PALETTES_HEIGHT * std::max(1, zoom - 1);
  int palettes_width = PALETTES_WIDTH * zoom;
  int palettes_height = PALETTES_HEIGHT * zoom;
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
  objects_window.setSize(sf::Vector2u(objects_width, objects_height));
  palettes_window.setSize(sf::Vector2u(palettes_width, palettes_height));

  // compute aggregate dimensions
  int total_windows_width = std::max(objects_width, palettes_width) +
                            pat_table_size + screen_width +
                            2 * nametable_width + 4 * padding;

  int pat_tables_height = 2 * pat_table_size + padding + TITLEBAR_HEIGHT;
  int nametables_height = 2 * nametable_height + padding + TITLEBAR_HEIGHT;

  // position sprites window
  int objs_x = (width - total_windows_width) / 2;
  int objs_y = (height - (objects_height + palettes_height + padding +
                          2 * TITLEBAR_HEIGHT)) /
               2;
  objects_window.setPosition(sf::Vector2i(objs_x, objs_y + TITLEBAR_HEIGHT));

  // position palettes window
  int pal_x = (width - total_windows_width) / 2;
  palettes_window.setPosition(sf::Vector2i(
      pal_x, objs_y + objects_height + padding + 2 * TITLEBAR_HEIGHT));

  // position pattern tables
  int pt_x = pal_x + palettes_width + padding;
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
      sf::Vector2i(nt_x + nametable_width + padding, nt_y));
  nametable3_window.setPosition(
      sf::Vector2i(nt_x, nt_y + nametable_height + padding + TITLEBAR_HEIGHT));
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

  objects_texture.create(SPRITES_WIDTH, SPRITES_HEIGHT);

  palettes_texture.create(PALETTES_WIDTH, PALETTES_HEIGHT);

  // define sprites
  window_sprite = sf::Sprite(texture);
  pt1_sprite = sf::Sprite(pt1_texture);
  pt2_sprite = sf::Sprite(pt2_texture);
  nt1_sprite = sf::Sprite(nt1_texture);
  nt2_sprite = sf::Sprite(nt2_texture);
  nt3_sprite = sf::Sprite(nt3_texture);
  nt4_sprite = sf::Sprite(nt4_texture);
  objects_sprite = sf::Sprite(objects_texture);
  palettes_sprite = sf::Sprite(palettes_texture);

  // audio
  SDL_Init(SDL_INIT_AUDIO);
  SDL_zero(audio_spec);
  audio_spec.freq = 44100;
  audio_spec.format = AUDIO_S16;
  audio_spec.channels = 1;
  audio_spec.samples = 1024;
  audio_spec.callback = NULL;
  audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
}

void Nes::Run() {
  // start cpu
  cpu.Startup();
  // change palette to FCEUX
  cpu.UseFceuxPalette();
  // display windows
  InitialDraw();
  // define clock
  sf::Clock clock;
  // time delta
  float dt = 0.0F;
  // total elapsed time
  float elapsed = 0.0F;

  SDL_PauseAudioDevice(audio_device, 0);

  while (window.isOpen()) {
    // run emulation forward
    if ((elapsed = clock.getElapsedTime().asSeconds() + dt) >= TIME_PER_FRAME) {
      dt = elapsed - TIME_PER_FRAME;
      clock.restart();
      Emulate();
    }
  }
}

void Nes::Test(uint64_t num_frames, std::string filepath) {
  // start cpu
  cpu.Startup();
  // change palette to FCEUX
  cpu.UseFceuxPalette();

  while (frames < num_frames) {
    switch (cpu.RunTillEvent(MAX_CYCLES)) {
      case cpu::Event::VBlank:
        frames++;
        break;
      case cpu::Event::MaxCycles:
        break;
      case cpu::Event::AudioBufferFull:
        cpu.GetAudioBuffer();
        break;
      case cpu::Event::Stopped:
        throw "Emulator Stopped";
    }
  }

  texture.update(cpu.GetScreen());
  if (texture.copyToImage().saveToFile(filepath)) {
    std::cout << "Screenshot saved to " << filepath << std::endl;
  }
}

void Nes::TestAudio(uint64_t num_frames, std::string filepath) {
  // start cpu
  cpu.Startup();
  // change palette to FCEUX
  cpu.UseFceuxPalette();
  // open audio file
  std::ofstream audio_file;
  audio_file.open(filepath);

  while (frames < num_frames) {
    switch (cpu.RunTillEvent(MAX_CYCLES)) {
      case cpu::Event::VBlank:
        frames++;
        break;
      case cpu::Event::MaxCycles:
        break;
      case cpu::Event::AudioBufferFull: {
        auto buffer = cpu.GetAudioBuffer();
        for (int16_t word : buffer) {
          // little-endian
          audio_file << static_cast<int8_t>(word);
          audio_file << static_cast<int8_t>(word >> 8);
        }
        break;
      }
      case cpu::Event::Stopped:
        std::cout << "Emulator Stopped exception in TestAudio" << std::endl;
        throw "Emulator Stopped";
    }
  }
}

void Nes::Emulate() {
  while (true) {
    switch (cpu.RunTillEvent(MAX_CYCLES)) {
      case cpu::Event::VBlank:
        HandleEvents();
        UpdateWindows();
        break;
      case cpu::Event::MaxCycles:
        return;
      case cpu::Event::AudioBufferFull:
        QueueAudio();
        break;
      case cpu::Event::Stopped:
        std::cout << "Emulator Stopped" << std::endl;
        throw "Emulator Stopped";
    }
  }
}

void Nes::QueueAudio() {
  auto samples = cpu.GetAudioBuffer();
  SDL_QueueAudio(audio_device, samples.data(),
                 sizeof(int16_t) * samples.size());
}

void Nes::HandleEvents() {
  while (window.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::KeyPressed:
        HandleKeyPress();
        break;
      case sf::Event::KeyReleased:
        HandleKeyRelease();
        break;
      default:
        return;
    }
  }
}

void Nes::HandleKeyPress() {
  switch (event.key.code) {
    case sf::Keyboard::A:
    case sf::Keyboard::S:
    case sf::Keyboard::Space:
    case sf::Keyboard::Enter:
    case sf::Keyboard::Up:
    case sf::Keyboard::Down:
    case sf::Keyboard::Left:
    case sf::Keyboard::Right:
      cpu.p1_input |= (1 << key_offsets[event.key.code]);
      break;
    case sf::Keyboard::LSystem:
    case sf::Keyboard::RSystem:
      cmd_pressed = true;
      break;
    case sf::Keyboard::Q:
    case sf::Keyboard::W:
      if (cmd_pressed) {
        window.close();
      }
      break;
    default:
      break;
  }
}

void Nes::HandleKeyRelease() {
  switch (event.key.code) {
    case sf::Keyboard::A:
    case sf::Keyboard::S:
    case sf::Keyboard::Space:
    case sf::Keyboard::Enter:
    case sf::Keyboard::Up:
    case sf::Keyboard::Down:
    case sf::Keyboard::Left:
    case sf::Keyboard::Right:
      cpu.p1_input &= ~(1 << key_offsets[event.key.code]);
      break;
    case sf::Keyboard::LSystem:
    case sf::Keyboard::RSystem:
      cmd_pressed = false;
      break;
    default:
      break;
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
  objects_texture.update(cpu.GetSprites());
  palettes_texture.update(cpu.GetPalettes());

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
  objects_window.draw(objects_sprite);
  palettes_window.draw(palettes_sprite);
}

void Nes::DisplayWindows() {
  window.display();

  pat_table1_window.display();
  pat_table2_window.display();

  nametable1_window.display();
  nametable2_window.display();
  nametable3_window.display();
  nametable4_window.display();

  objects_window.display();

  palettes_window.display();
}

}  // namespace nes
