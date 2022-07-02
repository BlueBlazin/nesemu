#ifndef SRC_CONTROLLERS_CONTROLLERS_H_
#define SRC_CONTROLLERS_CONTROLLERS_H_

#include <cstdint>

namespace controllers {

enum class Key {
  A,
  B,
  Select,
  Start,
  Up,
  Down,
  Left,
  Right,
};

class Controller {
 public:
  Controller();

  void PressKey(Key key);
  void ReleaseKey(Key key);

  void SetStrobe(uint8_t value);
  uint8_t Read();

 private:
  bool polling = false;
  int clock = 0;
  uint8_t keys[8];
};

}  // namespace controllers

#endif  // SRC_CONTROLLERS_CONTROLLERS_H_