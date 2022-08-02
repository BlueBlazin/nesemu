#include "apu.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>

#include "src/apu/pulse.h"

namespace audio {

Apu::Apu(std::shared_ptr<mappers::Mapper> mapper)
    : audio_buffer(),
      pulse1(PulseChannel::Pulse1),
      pulse2(PulseChannel::Pulse2),
      triangle(),
      noise(),
      dmc(std::move(mapper)) {}

void Apu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    cycles--;

    ClockSequencer();

    // clock channels
    pulse1.Clock();
    pulse2.Clock();
    triangle.Clock();
    noise.Clock();
    dmc.Clock();

    sample_counter += 1.0F;
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

  half_cycles++;
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
  triangle.ClockLinear();
  noise.envelope.Clock();
}

void Apu::ClockLengthAndSweep() {
  pulse1.length_counter.Clock();
  pulse2.length_counter.Clock();
  triangle.length_counter.Clock();
  noise.length_counter.Clock();

  pulse1.sweep.Clock();
  pulse2.sweep.Clock();
}

void Apu::Sample() {
  uint16_t pulse1_out = pulse1_enabled ? pulse1.Volume() : 0;
  uint16_t pulse2_out = pulse2_enabled ? pulse2.Volume() : 0;
  uint16_t triangle_out = triangle_enabled ? triangle.Volume() : 0;
  uint16_t noise_out = noise_enabled ? noise.Volume() : 0;
  uint16_t dmc_out = dmc.Volume();

  double pulse_out = MIXER_PULSE_TABLE[pulse1_out + pulse2_out];
  double tnd_out = MIXER_TND_TABLE[3 * triangle_out + 2 * noise_out + dmc_out];
  double output = pulse_out + tnd_out;

  audio_buffer.push_back(INT16_MAX * (output - 0.5));
}

/* Debug Noise */
// void Apu::Sample() {
//   uint16_t noise_out = noise_enabled ? noise.Volume() : 0;
//   double tnd_out = MIXER_TND_TABLE[2 * noise_out];
//   double output = tnd_out;

//   // audio_buffer.push_back(INT16_MAX * (output - 0.09) / 0.045);
//   audio_buffer.push_back(INT16_MAX * output);
// }

bool Apu::AudioBufferFull() { return audio_buffer.size() >= AUDIO_BUFFER_SIZE; }

std::vector<int16_t> Apu::GetAudioBuffer() {
  return std::exchange(audio_buffer, std::vector<int16_t>());
}

uint8_t Apu::Read(uint16_t addr) {
  switch (addr) {
    case 0x4015: {
      uint8_t value =
          (static_cast<uint8_t>(dmc.dmc_interrupt) << 7) |
          (static_cast<uint8_t>(frame_interrupt) << 6) |
          /*                                 */ 0 << 5 |
          (static_cast<uint8_t>(dmc.bytes_remaining > 0) << 4) |
          (static_cast<uint8_t>(noise.length_counter.NonZero()) << 3) |
          (static_cast<uint8_t>(triangle.length_counter.NonZero()) << 2) |
          (static_cast<uint8_t>(pulse2.length_counter.NonZero()) << 1) |
          (static_cast<uint8_t>(pulse1.length_counter.NonZero()) << 0);

      frame_interrupt = false;
      return value;
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
      triangle.Write(addr, value);
      break;
    case 0x400C:
    case 0x400D:
    case 0x400E:
    case 0x400F:
      noise.Write(addr, value);
      break;
    case 0x4010:
    case 0x4011:
    case 0x4012:
    case 0x4013:
      dmc.Write(addr, value);
      break;
    case 0x4015: {
      dmc_enabled = static_cast<bool>(value & 0x10);
      noise_enabled = static_cast<bool>(value & 0x8);
      triangle_enabled = static_cast<bool>(value & 0x4);
      pulse2_enabled = static_cast<bool>(value & 0x2);
      pulse1_enabled = static_cast<bool>(value & 0x1);

      pulse1.length_counter.SetEnabled(pulse1_enabled);
      pulse2.length_counter.SetEnabled(pulse2_enabled);
      triangle.length_counter.SetEnabled(triangle_enabled);
      noise.length_counter.SetEnabled(noise_enabled);
      dmc.SetEnabled(dmc_enabled);

      dmc.dmc_interrupt = false;
      break;
    }
    case 0x4017: {
      mode0 = (value >> 7) == 0;
      interrupt_inhibit = static_cast<bool>(value & 0x40);

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
