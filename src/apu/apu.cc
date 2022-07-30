#include "apu.h"

#include <cstdint>
#include <iostream>
#include <utility>

#include "src/apu/pulse.h"

namespace audio {

Apu::Apu()
    : audio_buffer(),
      pulse1(PulseChannel::Pulse1),
      pulse2(PulseChannel::Pulse2) {
  std::cout << "SAMPLE_CLOCKS: " << SAMPLE_CLOCKS << std::endl;
}

void Apu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    cycles--;
    half_cycles++;

    // clock channels
    pulse1.Clock();
    pulse2.Clock();

    ClockSequencer();

    sample_counter += 1.0;
    if (sample_counter >= SAMPLE_CLOCKS) {
      sample_counter -= SAMPLE_CLOCKS;
      Sample();
    }
  }
}

void Apu::ClockSequencer() {
  if (frame_reset_delay > 0) {
    frame_reset_delay--;

    if (frame_reset_delay == 0) {
      half_cycles = 0;

      if (!mode0) {
        ClockEnvelopesAndLinear();
        ClockLengthAndSweep();
      }
    }
  }

  if (mode0) {
    ModeZeroTick();
  } else {
    ModeOneTick();
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
  pulse1.envelope.Clock();
  pulse2.envelope.Clock();
}

void Apu::ClockLengthAndSweep() {
  pulse1.length_counter.Clock();
  pulse2.length_counter.Clock();

  pulse1.sweep.Clock();
  pulse2.sweep.Clock();
}

void Apu::Sample() {
  uint16_t pulse1_out = pulse1_enabled ? pulse1.Volume() : 0;
  uint16_t pulse2_out = pulse2_enabled ? pulse2.Volume() : 0;

  double pulse_out = MIXER_PULSE_TABLE[pulse1_out + pulse2_out];

  audio_buffer.push_back(INT16_MAX * (pulse_out - 0.5));
}

bool Apu::AudioBufferFull() { return audio_buffer.size() >= AUDIO_BUFFER_SIZE; }

std::vector<int16_t> Apu::GetAudioBuffer() {
  return std::exchange(audio_buffer, std::vector<int16_t>());
}

uint8_t Apu::Read(uint16_t addr) {
  switch (addr) {
    case 0x4015: {
      frame_interrupt = false;

      return 0x00 | (static_cast<uint8_t>(dmc_interrupt) << 7) |
             (static_cast<uint8_t>(frame_interrupt) << 6) |
             (static_cast<uint8_t>(dmc_enabled) << 4) |
             (/* noise length counter > 0 */ 0 << 3) |
             (/* triangle length counter > 0 */ 0 << 2) |
             (static_cast<uint8_t>(pulse2.length_counter.NonZero()) << 1) |
             (static_cast<uint8_t>(pulse1.length_counter.NonZero()) << 0);
    }
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
      pulse1.Write(addr, value);
      break;
    case 0x4004:
    case 0x4005:
    case 0x4006:
    case 0x4007:
      pulse2.Write(addr, value);
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
    case 0x4015: {
      dmc_enabled = static_cast<bool>(value & 0x10);
      noise_enabled = static_cast<bool>(value & 0x8);
      triangle_enabled = static_cast<bool>(value & 0x4);
      pulse2_enabled = static_cast<bool>(value & 0x2);
      pulse1_enabled = static_cast<bool>(value & 0x1);

      pulse1.length_counter.SetEnabled(pulse1_enabled);
      pulse2.length_counter.SetEnabled(pulse2_enabled);

      // TODO: check and reset length counters for other channels

      dmc_interrupt = false;
      break;
    }
    case 0x4017: {
      mode0 = (value >> 7) == 0;
      interrupt_inhibit = static_cast<bool>((value >> 6) & 0x1);

      if (half_cycles % 2 == 1) {
        frame_reset_delay = 3;
      } else {
        frame_reset_delay = 4;
      }

      if (interrupt_inhibit) {
        frame_interrupt = false;
      }

      break;
    }
  }
}

}  // namespace audio
