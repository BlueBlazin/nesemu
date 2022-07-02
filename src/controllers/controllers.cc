#include "controllers.h"

#include <iostream>

namespace controllers {

Controller::Controller() : keys{0, 0, 0, 0, 0, 0, 0, 0} {}

void Controller::PressKey(Key key) {
  switch (key) {
    case Key::A:
      keys[0] = 1;
      return;
    case Key::B:
      keys[1] = 1;
      return;
    case Key::Select:
      keys[2] = 1;
      return;
    case Key::Start:
      keys[3] = 1;
      return;
    case Key::Up:
      keys[4] = 1;
      return;
    case Key::Down:
      keys[5] = 1;
      return;
    case Key::Left:
      keys[6] = 1;
      return;
    case Key::Right:
      keys[7] = 1;
      return;
  }
}

void Controller::ReleaseKey(Key key) {
  return;

  switch (key) {
    case Key::A:
      keys[0] = 0;
      return;
    case Key::B:
      keys[1] = 0;
      return;
    case Key::Select:
      keys[2] = 0;
      return;
    case Key::Start:
      keys[3] = 0;
      return;
    case Key::Up:
      keys[4] = 0;
      return;
    case Key::Down:
      keys[5] = 0;
      return;
    case Key::Left:
      keys[6] = 0;
      return;
    case Key::Right:
      keys[7] = 0;
      return;
  }
}

void Controller::SetStrobe(uint8_t value) {
  polling = static_cast<bool>(value & 0x1);
}

uint8_t Controller::Read() {
  if (polling) {
    clock = 0;
    return keys[0];
  } else if (clock >= 8) {
    return 0x00;
  } else {
    uint8_t pressed = keys[clock];
    keys[clock++] = 0;
    return pressed;
  }
}

}  // namespace controllers