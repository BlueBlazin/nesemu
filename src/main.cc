#include <string>

#include "src/nes/nes.h"

int main(int argc, char* argv[]) {
  if (std::string(argv[1]) == std::string("test")) {
    nes::Nes emulator(argv[2]);
    emulator.Test(500, argv[3]);
  } else {
    nes::Nes emulator(argv[1]);
    emulator.Run();
  }
}
