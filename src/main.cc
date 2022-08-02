#include <string>

#include "src/nes/nes.h"

int main(int argc, char* argv[]) {
  if (std::string(argv[1]) == std::string("test")) {
    nes::Nes emulator(argv[2]);
    emulator.Test(500, argv[3]);
  } else if (std::string(argv[1]) == std::string("audio")) {
    nes::Nes emulator(argv[2]);
    emulator.TestAudio(700, argv[3]);
  } else {
    nes::Nes emulator(argv[1]);
    emulator.Run();
  }
}
