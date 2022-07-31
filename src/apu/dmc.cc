#include "dmc.h"

namespace audio {

Dmc::Dmc(std::shared_ptr<mappers::Mapper> mapper) : cartridge(mapper) {}

void Dmc::Clock() {
  if (timer == 0) {
    timer = rate;
    ClockOutputCycle();
  } else {
    timer--;
  }

  if (sample_buffer_emptied && bytes_remaining > 0) {
    stall_cpu = true;
    NextSampleByte();
  } else {
    stall_cpu = false;
  }
}

void Dmc::ClockOutputCycle() {
  if (!silence) {
    if (static_cast<bool>(shift_register & 0x1)) {
      if (output_level <= 125) {
        output_level += 2;
      }
    } else {
      if (output_level >= 2) {
        output_level -= 2;
      }
    }
  }

  shift_register = shift_register >> 1;
  bits_remaining--;

  if (bits_remaining == 0) {
    StartNewOutputCycle();
  }
}

void Dmc::StartNewOutputCycle() {
  bits_remaining = 8;

  if (sample_buffer_emptied) {
    silence = true;
  } else {
    silence = false;
    shift_register = sample_buffer;
    sample_buffer_emptied = true;
  }
}

void Dmc::NextSampleByte() {
  if (restart_pending) {
    restart_pending = false;
    RestartSample();
  }

  sample_buffer = cartridge->CpuRead(sample_addr);
  sample_buffer_emptied = false;
  sample_addr = (sample_addr == 0xFFFF) ? (0x8000) : (sample_addr + 1);
  bytes_remaining--;

  if (bytes_remaining == 0) {
    if (loop) {
      // restart
      RestartSample();
    } else if (irq_enable) {
      // request DMC interrupt
      dmc_interrupt = true;
    }
  }
}

void Dmc::RestartSample() {
  sample_addr = 0xC000 + (sample_addr_raw * 64);
  bytes_remaining = (sample_length_raw * 16) + 1;
}

void Dmc::SetEnabled(bool value) {
  if (!value) {
    bytes_remaining = 0;
  } else {
    if (bytes_remaining == 0) {
      RestartSample();
    } else {
      restart_pending = true;
    }
  }
}

uint16_t Dmc::Volume() { return output_level; }

void Dmc::Write(uint16_t addr, uint8_t value) {
  switch (addr) {
    case 0x4010: {
      irq_enable = static_cast<bool>(value & 0x80);
      loop = static_cast<bool>(value & 0x40);
      rate = RATES[value & 0xF];
      timer = rate;
      break;
    }
    case 0x4011: {
      output_level = static_cast<uint16_t>(value & 0x7F);
      break;
    }
    case 0x4012: {
      sample_addr_raw = static_cast<uint16_t>(value);
      sample_addr = 0xC000 + (sample_addr_raw * 64);
      break;
    }
    case 0x4013: {
      sample_length_raw = static_cast<uint16_t>(value);
      bytes_remaining = (sample_length_raw * 16) + 1;
      break;
    }
    default:
      break;
  }
}

}  // namespace audio
