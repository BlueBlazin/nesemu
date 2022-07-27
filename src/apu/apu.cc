#include "apu.h"

#include <cstdint>
#include <utility>

#include "src/apu/pulse.h"

namespace audio {

Apu::Apu()
    : audio_buffer(),
      pulse1(PulseChannel::Pulse1),
      pulse2(PulseChannel::Pulse2) {}

void Apu::Tick(uint64_t cycles) {
  while (cycles > 0) {
    cycles--;
    half_cycles++;

    if (mode0) {
      ModeZeroTick();
    } else {
      ModeOneTick();
    }

    sample_counter++;
    if (sample_counter >= SAMPLE_CLOCKS) {
      sample_counter = 0;
      Sample();
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
  uint16_t pulse1_out = pulse1.Volume();
  uint16_t pulse2_out = pulse2.Volume();

  double pulse_out = MIXER_PULSE_TABLE[pulse1_out + pulse2_out];

  audio_buffer.push_back(INT16_MAX * (pulse_out - 0.5));
}

bool Apu::AudioBufferFull() { return audio_buffer.size() >= AUDIO_BUFFER_SIZE; }

std::vector<int16_t> Apu::GetAudioBuffer() {
  return std::exchange(audio_buffer, std::vector<int16_t>());
}

uint8_t Apu::Read(uint16_t addr) {
  switch (addr) {
    case 0x4000:
    case 0x4001:
    case 0x4002:
    case 0x4003:
      return 0x00;
    case 0x4004:
    case 0x4005:
    case 0x4006:
    case 0x4007:
      return 0x00;
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
