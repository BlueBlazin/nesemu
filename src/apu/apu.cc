#include "apu.h"

#include <cstdint>

namespace audio {

Apu::Apu() {}

void Apu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    cycles--;
    half_cycles++;

    if (mode0) {
      ModeZeroTick();
    } else {
      ModeOneTick();
    }
  }
}

void Apu::ModeZeroTick() {
  switch (half_cycles) {
    case STEP1: {
      ClockEnvelopesAndLinear();
      break;
    }
    case STEP2: {
      ClockEnvelopesAndLinear();
      ClockLengthAndSweep();
      break;
    }
    case STEP3: {
      ClockEnvelopesAndLinear();
      break;
    }
    case STEP4_1: {
      if (!interrupt_inhibit) {
        frame_interrupt = true;
      }
      break;
    }
    case STEP4_2: {
      ClockEnvelopesAndLinear();
      ClockLengthAndSweep();
      if (!interrupt_inhibit) {
        frame_interrupt = true;
      }
      break;
    }
    case MODE0_RESET: {
      if (!interrupt_inhibit) {
        frame_interrupt = true;
      }
      half_cycles = 0;
      break;
    }
    default:
      break;
  }
}

void Apu::ModeOneTick() {
  switch (half_cycles) {
    case STEP1: {
      ClockEnvelopesAndLinear();
      break;
    }
    case STEP2: {
      ClockEnvelopesAndLinear();
      ClockLengthAndSweep();
      break;
    }
    case STEP3: {
      ClockEnvelopesAndLinear();
      break;
    }
    case STEP5: {
      ClockEnvelopesAndLinear();
      ClockLengthAndSweep();
      break;
    }
    case MODE1_RESET: {
      half_cycles = 0;
      break;
    }
    default:
      break;
  }
}

void Apu::ClockEnvelopesAndLinear() {
  //
}

void Apu::ClockLengthAndSweep() {
  //
}

uint8_t Apu::Read(uint16_t addr) {
  switch (addr) {
    case 0x4000:
    case 0x4001:
    case 0x4002:
    case 0x4003:
      // pulse 1
      break;
    case 0x4004:
    case 0x4005:
    case 0x4006:
    case 0x4007:
      // pulse 2
      break;
    case 0x4008:
    case 0x4009:
    case 0x400A:
    case 0x400B:
      // Triangle
      break;
    case 0x400C:
    case 0x400D:
    case 0x400E:
    case 0x400F:
      // Noise
      break;
    case 0x4010:
    case 0x4011:
    case 0x4012:
    case 0x4013:
      // Noise
      break;
    case 0x4015:
      // TODO
      break;
    default:
      return 0x00;
  }
}

void Apu::Write(uint16_t addr, uint8_t value) {
  switch (addr) {
    case 0x4000:
    case 0x4001:
    case 0x4002:
    case 0x4003:
      // pulse 1
      break;
    case 0x4004:
    case 0x4005:
    case 0x4006:
    case 0x4007:
      // pulse 2
      break;
    case 0x4008:
    case 0x4009:
    case 0x400A:
    case 0x400B:
      // Triangle
      break;
    case 0x400C:
    case 0x400D:
    case 0x400E:
    case 0x400F:
      // Noise
      break;
    case 0x4010:
    case 0x4011:
    case 0x4012:
    case 0x4013:
      // Noise
      break;
    case 0x4015:
      // TODO
      break;
    case 0x4017: {
      mode0 = static_cast<bool>(value >> 7);
      interrupt_inhibit = static_cast<bool>((value >> 6) & 0x1);
      break;
    }
  }
}

}  // namespace audio
