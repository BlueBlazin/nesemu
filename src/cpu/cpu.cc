#include "cpu.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "src/cpu/event.h"
#include "src/memory/memory.h"

namespace cpu {

Cpu::Cpu(const std::string& path) : mmu(path, p1_input) {
  myfile.open("emu.log");
}

void Cpu::Startup() {
  PC = static_cast<uint16_t>(mmu.Read(0xFFFC));
  PC |= static_cast<uint16_t>(mmu.Read(0xFFFD)) << 8;
}

Event Cpu::RunTillEvent(uint64_t max_cycles) {
  if (stopped) {
    return Event::Stopped;
  }

  while (event_cycles < max_cycles) {
    Tick();

    if (mmu.VblankEvent()) {
      mmu.ClearVBlankEvent();
      return Event::VBlank;
    } else if (mmu.apu.AudioBufferFull()) {
      return Event::AudioBufferFull;
    }
  }

  event_cycles -= max_cycles;
  return Event::MaxCycles;
}

void Cpu::Run() {
  /* fetch-decode-execute loop */
  while (true) {
    Tick();
  }
}

void Cpu::Tick() {
  if (mmu.InDma()) {
    RunDma();
    return;
  }

  // Check for interrupts
  if (NmiPending()) {
    mmu.ClearNmi();
    Interrupt(InterruptType::Nmi);
  } else if (!flag_I && IrqPending()) {
    Interrupt(InterruptType::Irq);
  } else {
    DecodeExecute(opcode = Fetch());
  }
}

uint8_t* Cpu::GetScreen() { return mmu.GetScreen(); }

uint8_t* Cpu::GetPatTable1() { return mmu.GetPatTable1(); }
uint8_t* Cpu::GetPatTable2() { return mmu.GetPatTable2(); }
uint8_t* Cpu::GetNametable(uint16_t addr) { return mmu.GetNametable(addr); }
uint8_t* Cpu::GetSprites() { return mmu.GetSprites(); }
uint8_t* Cpu::GetPalettes() { return mmu.GetPalettes(); }

void Cpu::RunDma() {
  if (dma_state == DmaState::PreDma) {
    dma_state = cycles % 2 == 1 ? DmaState::OddCycleWait : DmaState::Running;
  } else if (dma_state == DmaState::OddCycleWait) {
    dma_state = DmaState::Running;
  } else {
    mmu.DmaTick();
  }

  AddCycle();
}

void Cpu::DecodeExecute(uint8_t opcode) {
  switch (opcode) {
    case 0x00:
      BrkImplied();
      break;
    case 0x01:
      OraIndirectX();
      break;
    case 0x02:
      Stp();
      break;
    case 0x03:
      SloIndirectX();
      break;
    case 0x04:
      NopZeroPage();
      break;
    case 0x05:
      OraZeroPage();
      break;
    case 0x06:
      AslZeroPage();
      break;
    case 0x07:
      SloZeroPage();
      break;
    case 0x08:
      PhpImplied();
      break;
    case 0x09:
      OraImmediate();
      break;
    case 0x0A:
      AslAccumulator();
      break;
    case 0x0B:
      AncImmediate();
      break;
    case 0x0C:
      NopAbsolute();
      break;
    case 0x0D:
      OraAbsolute();
      break;
    case 0x0E:
      AslAbsolute();
      break;
    case 0x0F:
      SloAbsolute();
      break;
    case 0x10:
      BplRelative();
      break;
    case 0x11:
      OraIndirectY();
      break;
    case 0x12:
      Stp();
      break;
    case 0x13:
      SloIndirectY();
      break;
    case 0x14:
      NopZeroPageX();
      break;
    case 0x15:
      OraZeroPageX();
      break;
    case 0x16:
      AslZeroPageX();
      break;
    case 0x17:
      SloZeroPageX();
      break;
    case 0x18:
      ClcImplied();
      break;
    case 0x19:
      OraAbsoluteY();
      break;
    case 0x1A:
      NopImplied();
      break;
    case 0x1B:
      SloAbsoluteY();
      break;
    case 0x1C:
      NopAbsoluteX();
      break;
    case 0x1D:
      OraAbsoluteX();
      break;
    case 0x1E:
      AslAbsoluteX();
      break;
    case 0x1F:
      SloAbsoluteX();
      break;
    case 0x20:
      JsrAbsolute();
      break;
    case 0x21:
      AndIndirectX();
      break;
    case 0x22:
      Stp();
      break;
    case 0x23:
      RlaIndirectX();
      break;
    case 0x24:
      BitZeroPage();
      break;
    case 0x25:
      AndZeroPage();
      break;
    case 0x26:
      RolZeroPage();
      break;
    case 0x27:
      RlaZeroPage();
      break;
    case 0x28:
      PlpImplied();
      break;
    case 0x29:
      AndImmediate();
      break;
    case 0x2A:
      RolAccumulator();
      break;
    case 0x2B:
      AncImmediate();
      break;
    case 0x2C:
      BitAbsolute();
      break;
    case 0x2D:
      AndAbsolute();
      break;
    case 0x2E:
      RolAbsolute();
      break;
    case 0x2F:
      RlaAbsolute();
      break;
    case 0x30:
      BmiRelative();
      break;
    case 0x31:
      AndIndirectY();
      break;
    case 0x32:
      Stp();
      break;
    case 0x33:
      RlaIndirectY();
      break;
    case 0x34:
      NopZeroPageX();
      break;
    case 0x35:
      AndZeroPageX();
      break;
    case 0x36:
      RolZeroPageX();
      break;
    case 0x37:
      RlaZeroPageX();
      break;
    case 0x38:
      SecImplied();
      break;
    case 0x39:
      AndAbsoluteY();
      break;
    case 0x3A:
      NopImplied();
      break;
    case 0x3B:
      RlaAbsoluteY();
      break;
    case 0x3C:
      NopAbsoluteX();
      break;
    case 0x3D:
      AndAbsoluteX();
      break;
    case 0x3E:
      RolAbsoluteX();
      break;
    case 0x3F:
      RlaAbsoluteX();
      break;
    case 0x40:
      RtiImplied();
      break;
    case 0x41:
      EorIndirectX();
      break;
    case 0x42:
      Stp();
      break;
    case 0x43:
      SreIndirectX();
      break;
    case 0x44:
      NopZeroPage();
      break;
    case 0x45:
      EorZeroPage();
      break;
    case 0x46:
      LsrZeroPage();
      break;
    case 0x47:
      SreZeroPage();
      break;
    case 0x48:
      PhaImplied();
      break;
    case 0x49:
      EorImmediate();
      break;
    case 0x4A:
      LsrAccumulator();
      break;
    case 0x4B:
      AlrImmediate();
      break;
    case 0x4C:
      JmpAbsolute();
      break;
    case 0x4D:
      EorAbsolute();
      break;
    case 0x4E:
      LsrAbsolute();
      break;
    case 0x4F:
      SreAbsolute();
      break;
    case 0x50:
      BvcRelative();
      break;
    case 0x51:
      EorIndirectY();
      break;
    case 0x52:
      Stp();
      break;
    case 0x53:
      SreIndirectY();
      break;
    case 0x54:
      NopZeroPageX();
      break;
    case 0x55:
      EorZeroPageX();
      break;
    case 0x56:
      LsrZeroPageX();
      break;
    case 0x57:
      SreZeroPageX();
      break;
    case 0x58:
      CliImplied();
      break;
    case 0x59:
      EorAbsoluteY();
      break;
    case 0x5A:
      NopImplied();
      break;
    case 0x5B:
      SreAbsoluteY();
      break;
    case 0x5C:
      NopAbsoluteX();
      break;
    case 0x5D:
      EorAbsoluteX();
      break;
    case 0x5E:
      LsrAbsoluteX();
      break;
    case 0x5F:
      SreAbsoluteX();
      break;
    case 0x60:
      RtsImplied();
      break;
    case 0x61:
      AdcIndirectX();
      break;
    case 0x62:
      Stp();
      break;
    case 0x63:
      RraIndirectX();
      break;
    case 0x64:
      NopZeroPage();
      break;
    case 0x65:
      AdcZeroPage();
      break;
    case 0x66:
      RorZeroPage();
      break;
    case 0x67:
      RraZeroPage();
      break;
    case 0x68:
      PlaImplied();
      break;
    case 0x69:
      AdcImmediate();
      break;
    case 0x6A:
      RorAccumulator();
      break;
    case 0x6B:
      ArrImmediate();
      break;
    case 0x6C:
      JmpIndirect();
      break;
    case 0x6D:
      AdcAbsolute();
      break;
    case 0x6E:
      RorAbsolute();
      break;
    case 0x6F:
      RraAbsolute();
      break;
    case 0x70:
      BvsRelative();
      break;
    case 0x71:
      AdcIndirectY();
      break;
    case 0x72:
      Stp();
      break;
    case 0x73:
      RraIndirectY();
      break;
    case 0x74:
      NopZeroPageX();
      break;
    case 0x75:
      AdcZeroPageX();
      break;
    case 0x76:
      RorZeroPageX();
      break;
    case 0x77:
      RraZeroPageX();
      break;
    case 0x78:
      SeiImplied();
      break;
    case 0x79:
      AdcAbsoluteY();
      break;
    case 0x7A:
      NopImplied();
      break;
    case 0x7B:
      RraAbsoluteY();
      break;
    case 0x7C:
      NopAbsoluteX();
      break;
    case 0x7D:
      AdcAbsoluteX();
      break;
    case 0x7E:
      RorAbsoluteX();
      break;
    case 0x7F:
      RraAbsoluteX();
      break;
    case 0x80:
      NopImmediate();
      break;
    case 0x81:
      StaIndirectX();
      break;
    case 0x82:
      NopImmediate();
      break;
    case 0x83:
      SaxIndirectX();
      break;
    case 0x84:
      StyZeroPage();
      break;
    case 0x85:
      StaZeroPage();
      break;
    case 0x86:
      StxZeroPage();
      break;
    case 0x87:
      SaxZeroPage();
      break;
    case 0x88:
      DeyImplied();
      break;
    case 0x89:
      NopImmediate();
      break;
    case 0x8A:
      TxaImplied();
      break;
    case 0x8B:
      AneImmediate();
      break;
    case 0x8C:
      StyAbsolute();
      break;
    case 0x8D:
      StaAbsolute();
      break;
    case 0x8E:
      StxAbsolute();
      break;
    case 0x8F:
      SaxAbsolute();
      break;
    case 0x90:
      BccRelative();
      break;
    case 0x91:
      StaIndirectY();
      break;
    case 0x92:
      Stp();
      break;
    case 0x93:
      ShaIndirectY();
      break;
    case 0x94:
      StyZeroPageX();
      break;
    case 0x95:
      StaZeroPageX();
      break;
    case 0x96:
      StxZeroPageY();
      break;
    case 0x97:
      SaxZeroPageY();
      break;
    case 0x98:
      TyaImplied();
      break;
    case 0x99:
      StaAbsoluteY();
      break;
    case 0x9A:
      TxsImplied();
      break;
    case 0x9B:
      TasAbsoluteY();
      break;
    case 0x9C:
      ShyAbsoluteX();
      break;
    case 0x9D:
      StaAbsoluteX();
      break;
    case 0x9E:
      ShxAbsoluteY();
      break;
    case 0x9F:
      ShaAbsoluteY();
      break;
    case 0xA0:
      LdyImmediate();
      break;
    case 0xA1:
      LdaIndirectX();
      break;
    case 0xA2:
      LdxImmediate();
      break;
    case 0xA3:
      LaxIndirectX();
      break;
    case 0xA4:
      LdyZeroPage();
      break;
    case 0xA5:
      LdaZeroPage();
      break;
    case 0xA6:
      LdxZeroPage();
      break;
    case 0xA7:
      LaxZeroPage();
      break;
    case 0xA8:
      TayImplied();
      break;
    case 0xA9:
      LdaImmediate();
      break;
    case 0xAA:
      TaxImplied();
      break;
    case 0xAB:
      LxaImmediate();
      break;
    case 0xAC:
      LdyAbsolute();
      break;
    case 0xAD:
      LdaAbsolute();
      break;
    case 0xAE:
      LdxAbsolute();
      break;
    case 0xAF:
      LaxAbsolute();
      break;
    case 0xB0:
      BcsRelative();
      break;
    case 0xB1:
      LdaIndirectY();
      break;
    case 0xB2:
      Stp();
      break;
    case 0xB3:
      LaxIndirectY();
      break;
    case 0xB4:
      LdyZeroPageX();
      break;
    case 0xB5:
      LdaZeroPageX();
      break;
    case 0xB6:
      LdxZeroPageY();
      break;
    case 0xB7:
      LaxZeroPageY();
      break;
    case 0xB8:
      ClvImplied();
      break;
    case 0xB9:
      LdaAbsoluteY();
      break;
    case 0xBA:
      TsxImplied();
      break;
    case 0xBB:
      LasAbsoluteY();
      break;
    case 0xBC:
      LdyAbsoluteX();
      break;
    case 0xBD:
      LdaAbsoluteX();
      break;
    case 0xBE:
      LdxAbsoluteY();
      break;
    case 0xBF:
      LaxAbsoluteY();
      break;
    case 0xC0:
      CpyImmediate();
      break;
    case 0xC1:
      CmpIndirectX();
      break;
    case 0xC2:
      NopImmediate();
      break;
    case 0xC3:
      DcpIndirectX();
      break;
    case 0xC4:
      CpyZeroPage();
      break;
    case 0xC5:
      CmpZeroPage();
      break;
    case 0xC6:
      DecZeroPage();
      break;
    case 0xC7:
      DcpZeroPage();
      break;
    case 0xC8:
      InyImplied();
      break;
    case 0xC9:
      CmpImmediate();
      break;
    case 0xCA:
      DexImplied();
      break;
    case 0xCB:
      SbxImmediate();
      break;
    case 0xCC:
      CpyAbsolute();
      break;
    case 0xCD:
      CmpAbsolute();
      break;
    case 0xCE:
      DecAbsolute();
      break;
    case 0xCF:
      DcpAbsolute();
      break;
    case 0xD0:
      BneRelative();
      break;
    case 0xD1:
      CmpIndirectY();
      break;
    case 0xD2:
      Stp();
      break;
    case 0xD3:
      DcpIndirectY();
      break;
    case 0xD4:
      NopZeroPageX();
      break;
    case 0xD5:
      CmpZeroPageX();
      break;
    case 0xD6:
      DecZeroPageX();
      break;
    case 0xD7:
      DcpZeroPageX();
      break;
    case 0xD8:
      CldImplied();
      break;
    case 0xD9:
      CmpAbsoluteY();
      break;
    case 0xDA:
      NopImplied();
      break;
    case 0xDB:
      DcpAbsoluteY();
      break;
    case 0xDC:
      NopAbsoluteX();
      break;
    case 0xDD:
      CmpAbsoluteX();
      break;
    case 0xDE:
      DecAbsoluteX();
      break;
    case 0xDF:
      DcpAbsoluteX();
      break;
    case 0xE0:
      CpxImmediate();
      break;
    case 0xE1:
      SbcIndirectX();
      break;
    case 0xE2:
      NopImmediate();
      break;
    case 0xE3:
      IscIndirectX();
      break;
    case 0xE4:
      CpxZeroPage();
      break;
    case 0xE5:
      SbcZeroPage();
      break;
    case 0xE6:
      IncZeroPage();
      break;
    case 0xE7:
      IscZeroPage();
      break;
    case 0xE8:
      InxImplied();
      break;
    case 0xE9:
      SbcImmediate();
      break;
    case 0xEA:
      NopImplied();
      break;
    case 0xEB:
      SbcImmediate();
      break;
    case 0xEC:
      CpxAbsolute();
      break;
    case 0xED:
      SbcAbsolute();
      break;
    case 0xEE:
      IncAbsolute();
      break;
    case 0xEF:
      IscAbsolute();
      break;
    case 0xF0:
      BeqRelative();
      break;
    case 0xF1:
      SbcIndirectY();
      break;
    case 0xF2:
      Stp();
      break;
    case 0xF3:
      IscIndirectY();
      break;
    case 0xF4:
      NopZeroPageX();
      break;
    case 0xF5:
      SbcZeroPageX();
      break;
    case 0xF6:
      IncZeroPageX();
      break;
    case 0xF7:
      IscZeroPageX();
      break;
    case 0xF8:
      SedImplied();
      break;
    case 0xF9:
      SbcAbsoluteY();
      break;
    case 0xFA:
      NopImplied();
      break;
    case 0xFB:
      IscAbsoluteY();
      break;
    case 0xFC:
      NopAbsoluteX();
      break;
    case 0xFD:
      SbcAbsoluteX();
      break;
    case 0xFE:
      IncAbsoluteX();
      break;
    case 0xFF:
      IscAbsoluteX();
      break;
  }
}

/*=================================================================
*  Instructions
=================================================================*/

// /* 4 cycles */
uint16_t Cpu::IndirectX() {
  uint8_t ptr_lo = Fetch();
  uint8_t ptr_hi = 0x00;
  ReadMemory((static_cast<uint16_t>(ptr_hi) << 8) |
             static_cast<uint16_t>(ptr_lo));
  ptr_lo += X;
  uint8_t lo = ReadMemory((static_cast<uint16_t>(ptr_hi) << 8) |
                          static_cast<uint16_t>(ptr_lo));
  ptr_lo++;
  uint8_t hi = ReadMemory((static_cast<uint16_t>(ptr_hi) << 8) |
                          static_cast<uint16_t>(ptr_lo));

  return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
}

/* 3 or 4 cycles */
uint16_t Cpu::IndirectY() {
  uint8_t ptr_lo = Fetch();
  uint8_t ptr_hi = 0x00;
  uint8_t lo = ReadMemory((static_cast<uint16_t>(ptr_hi) << 8) |
                          static_cast<uint16_t>(ptr_lo));
  ptr_lo++;
  uint8_t hi = ReadMemory((static_cast<uint16_t>(ptr_hi) << 8) |
                          static_cast<uint16_t>(ptr_lo));
  bool inc = static_cast<uint16_t>(lo) + static_cast<uint16_t>(Y) > 0xFF;
  lo += Y;
  if (inc) {
    ReadMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo));
    hi++;
  }
  return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
}

/* 4 cycles */
uint16_t Cpu::IndirectYW() {
  uint8_t ptr_lo = Fetch();
  uint8_t ptr_hi = 0x00;
  uint8_t lo = ReadMemory((static_cast<uint16_t>(ptr_hi) << 8) |
                          static_cast<uint16_t>(ptr_lo));
  ptr_lo++;
  uint8_t hi = ReadMemory((static_cast<uint16_t>(ptr_hi) << 8) |
                          static_cast<uint16_t>(ptr_lo));
  bool inc = static_cast<uint16_t>(lo) + static_cast<uint16_t>(Y) > 0xFF;
  lo += Y;
  ReadMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo));
  if (inc) {
    hi++;
  }
  return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
}

/* 2 cycles */
uint16_t Cpu::ZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  ReadMemory(addr);
  return (addr + static_cast<uint16_t>(X)) & 0xFF;
}

/* 2 cycles */
uint16_t Cpu::ZeroPageY() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  ReadMemory(addr);
  return (addr + static_cast<uint16_t>(Y)) & 0xFF;
}

/* 1 cycle */
uint16_t Cpu::ZeroPage() { return static_cast<uint16_t>(Fetch()); }

/* 2 cycles */
uint16_t Cpu::Absolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  return (hi << 8) | lo;
}

/* 2 or 3 cycles */
uint16_t Cpu::AbsoluteX() {
  uint8_t lo = Fetch();
  uint8_t hi = Fetch();
  bool inc = static_cast<uint16_t>(lo) + static_cast<uint16_t>(X) > 0xFF;
  lo += X;
  if (inc) {
    ReadMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo));
    hi++;
  }
  return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
}

/* 2 or 3 cycles */
uint16_t Cpu::AbsoluteY() {
  uint8_t lo = Fetch();
  uint8_t hi = Fetch();
  bool inc = static_cast<uint16_t>(lo) + static_cast<uint16_t>(Y) > 0xFF;
  lo += Y;
  if (inc) {
    ReadMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo));
    hi++;
  }
  return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
}

/* 3 cycles */
uint16_t Cpu::AbsoluteXW() {
  uint8_t lo = Fetch();
  uint8_t hi = Fetch();
  bool inc = static_cast<uint16_t>(lo) + static_cast<uint16_t>(X) > 0xFF;
  lo += X;
  ReadMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo));
  if (inc) {
    hi++;
  }
  return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
}

/* 3 cycles */
uint16_t Cpu::AbsoluteYW() {
  uint8_t lo = Fetch();
  uint8_t hi = Fetch();
  bool inc = static_cast<uint16_t>(lo) + static_cast<uint16_t>(Y) > 0xFF;
  lo += Y;
  ReadMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo));
  if (inc) {
    hi++;
  }
  return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
}

/******************************************************************
  LDA
******************************************************************/
void Cpu::LdaImmediate() {
  A = Fetch();
  UpdateNZ(A);
}

void Cpu::LdaZeroPage() {
  uint16_t addr = ZeroPage();
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaZeroPageX() {
  uint16_t addr = ZeroPageX();
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaAbsolute() {
  uint16_t addr = Absolute();
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaAbsoluteX() {
  uint16_t addr = AbsoluteX();
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaAbsoluteY() {
  uint16_t addr = AbsoluteY();
  if (addr > 0xFF) {
    AddCycle();
  }
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaIndirectX() {
  uint16_t addr = IndirectX();
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaIndirectY() {
  uint16_t addr = IndirectY();
  A = ReadMemory(addr);
  UpdateNZ(A);
}

/******************************************************************
  LDX
******************************************************************/
void Cpu::LdxImmediate() {
  X = Fetch();
  UpdateNZ(X);
}

void Cpu::LdxZeroPage() { Ldx(ZeroPage()); }

void Cpu::LdxZeroPageY() { Ldx(ZeroPageY()); }

void Cpu::LdxAbsolute() { Ldx(Absolute()); }

void Cpu::LdxAbsoluteY() { Ldx(AbsoluteY()); }

void Cpu::Ldx(uint16_t addr) {
  X = ReadMemory(addr);
  UpdateNZ(X);
}

/******************************************************************
  LDY
******************************************************************/
void Cpu::LdyImmediate() {
  Y = Fetch();
  UpdateNZ(Y);
}

void Cpu::LdyZeroPage() { Ldy(ZeroPage()); }

void Cpu::LdyZeroPageX() { Ldy(ZeroPageX()); }

void Cpu::LdyAbsolute() { Ldy(Absolute()); }

void Cpu::LdyAbsoluteX() { Ldy(AbsoluteX()); }

void Cpu::Ldy(uint16_t addr) {
  Y = ReadMemory(addr);
  UpdateNZ(Y);
}

/******************************************************************
  STA
******************************************************************/
void Cpu::StaZeroPage() {
  uint16_t addr = ZeroPage();
  WriteMemory(addr, A);
}

void Cpu::StaZeroPageX() {
  uint16_t addr = ZeroPageX();
  WriteMemory(addr, A);
}

void Cpu::StaAbsolute() {
  uint16_t addr = Absolute();
  WriteMemory(addr, A);
}

void Cpu::StaAbsoluteX() {
  uint16_t addr = AbsoluteXW();
  WriteMemory(addr, A);
}

void Cpu::StaAbsoluteY() {
  uint16_t addr = AbsoluteYW();
  WriteMemory(addr, A);
}

void Cpu::StaIndirectX() {
  uint16_t addr = IndirectX();
  WriteMemory(addr, A);
}

void Cpu::StaIndirectY() {
  uint16_t addr = IndirectYW();
  WriteMemory(addr, A);
}

/******************************************************************
  STX
******************************************************************/
void Cpu::StxZeroPage() {
  uint16_t addr = ZeroPage();
  WriteMemory(addr, X);
}

void Cpu::StxZeroPageY() {
  uint16_t addr = ZeroPageY();
  WriteMemory(addr, X);
}

void Cpu::StxAbsolute() {
  uint16_t addr = Absolute();
  WriteMemory(addr, X);
}

/******************************************************************
  STY
******************************************************************/
void Cpu::StyZeroPage() {
  uint16_t addr = ZeroPage();
  WriteMemory(addr, Y);
}

void Cpu::StyZeroPageX() {
  uint16_t addr = ZeroPageX();
  WriteMemory(addr, Y);
}

void Cpu::StyAbsolute() {
  uint16_t addr = Absolute();
  WriteMemory(addr, Y);
}

/******************************************************************
  TAX
******************************************************************/
void Cpu::TaxImplied() {
  X = A;
  AddCycle();
  UpdateNZ(X);
}

/******************************************************************
  TAY
******************************************************************/
void Cpu::TayImplied() {
  Y = A;
  AddCycle();
  UpdateNZ(Y);
}

/******************************************************************
  TSX
******************************************************************/
void Cpu::TsxImplied() {
  X = SP;
  AddCycle();
  UpdateNZ(X);
}

/******************************************************************
  TXA
******************************************************************/
void Cpu::TxaImplied() {
  A = X;
  AddCycle();
  UpdateNZ(A);
}

/******************************************************************
  TSX
******************************************************************/
void Cpu::TxsImplied() {
  SP = X;
  AddCycle();
}

/******************************************************************
  TYA
******************************************************************/
void Cpu::TyaImplied() {
  A = Y;
  AddCycle();
  UpdateNZ(A);
}

/******************************************************************
  PHA
******************************************************************/
void Cpu::PhaImplied() {
  AddCycle();
  Push(A);
}

/******************************************************************
  PHP
******************************************************************/
void Cpu::PhpImplied() {
  AddCycle();
  Push((static_cast<uint8_t>(flag_N) << 7) |
       (static_cast<uint8_t>(flag_V) << 6) | (1 << 5) | (1 << 4) |
       (static_cast<uint8_t>(flag_D) << 3) |
       (static_cast<uint8_t>(flag_I) << 2) |
       (static_cast<uint8_t>(flag_Z) << 1) |
       (static_cast<uint8_t>(flag_C) << 0));
}

/******************************************************************
  PLA
******************************************************************/
void Cpu::PlaImplied() {
  AddCycle();
  SP++;
  AddCycle();
  A = Pull(SP);
  UpdateNZ(A);
}

/******************************************************************
  PLP
******************************************************************/
void Cpu::PlpImplied() {
  AddCycle();
  SP++;
  AddCycle();
  uint8_t status = Pull(SP);
  flag_N = static_cast<bool>((status >> 7) & 0x1);
  flag_V = static_cast<bool>((status >> 6) & 0x1);
  flag_D = static_cast<bool>((status >> 3) & 0x1);
  flag_I = static_cast<bool>((status >> 2) & 0x1);
  flag_Z = static_cast<bool>((status >> 1) & 0x1);
  flag_C = static_cast<bool>((status >> 0) & 0x1);
}

/******************************************************************
  DEC
******************************************************************/
void Cpu::DecZeroPage() {
  uint16_t addr = ZeroPage();
  Dec(addr, ReadMemory(addr));
}

void Cpu::DecZeroPageX() {
  uint16_t addr = ZeroPageX();
  Dec(addr, ReadMemory(addr));
}

void Cpu::DecAbsolute() {
  uint16_t addr = Absolute();
  Dec(addr, ReadMemory(addr));
}

void Cpu::DecAbsoluteX() {
  uint16_t addr = AbsoluteXW();
  Dec(addr, ReadMemory(addr));
}

void Cpu::Dec(uint16_t addr, uint8_t value) {
  WriteMemory(addr, value);
  value--;
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/******************************************************************
  DEX
******************************************************************/
void Cpu::DexImplied() {
  X--;
  AddCycle();
  UpdateNZ(X);
}

/******************************************************************
  DEY
******************************************************************/
void Cpu::DeyImplied() {
  Y--;
  AddCycle();
  UpdateNZ(Y);
}

/******************************************************************
  INC
******************************************************************/

void Cpu::IncZeroPage() {
  uint16_t addr = ZeroPage();
  Inc(addr, ReadMemory(addr));
}

void Cpu::IncZeroPageX() {
  uint16_t addr = ZeroPageX();
  Inc(addr, ReadMemory(addr));
}

void Cpu::IncAbsolute() {
  uint16_t addr = Absolute();
  Inc(addr, ReadMemory(addr));
}

void Cpu::IncAbsoluteX() {
  uint16_t addr = AbsoluteXW();
  Inc(addr, ReadMemory(addr));
}

void Cpu::Inc(uint16_t addr, uint8_t value) {
  WriteMemory(addr, value);
  value++;
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/******************************************************************
  INX
******************************************************************/
void Cpu::InxImplied() {
  X++;
  AddCycle();
  UpdateNZ(X);
}

/******************************************************************
  INY
******************************************************************/
void Cpu::InyImplied() {
  Y++;
  AddCycle();
  UpdateNZ(Y);
}

/******************************************************************
  ADC
******************************************************************/
void Cpu::AdcImmediate() { Adc(Fetch()); }

void Cpu::AdcZeroPage() {
  uint16_t addr = ZeroPage();
  Adc(ReadMemory(addr));
}

void Cpu::AdcZeroPageX() {
  uint16_t addr = ZeroPageX();
  Adc(ReadMemory(addr));
}

void Cpu::AdcAbsolute() {
  uint16_t addr = Absolute();
  Adc(ReadMemory(addr));
}

void Cpu::AdcAbsoluteX() {
  uint16_t addr = AbsoluteX();
  Adc(ReadMemory(addr));
}

void Cpu::AdcAbsoluteY() {
  uint16_t addr = AbsoluteY();
  Adc(ReadMemory(addr));
}

void Cpu::AdcIndirectX() {
  uint16_t addr = IndirectX();
  Adc(ReadMemory(addr));
}

void Cpu::AdcIndirectY() {
  uint16_t addr = IndirectY();
  Adc(ReadMemory(addr));
}

bool Cpu::Overflow(int8_t reg, int8_t value, int8_t carry) {
  int16_t result = static_cast<int16_t>(reg) + static_cast<int16_t>(value) +
                   static_cast<int16_t>(carry);
  return result < -128 || result > 127;
}

void Cpu::Adc(uint8_t value) {
  uint16_t result = static_cast<uint16_t>(A) + static_cast<uint16_t>(value) +
                    static_cast<uint16_t>(flag_C);
  flag_V = Overflow(static_cast<int8_t>(A), static_cast<int8_t>(value),
                    static_cast<int8_t>(flag_C));
  flag_C = result > 0xFF;
  A = static_cast<uint8_t>(result & 0xFF);
  flag_Z = A == 0;
  flag_N = static_cast<bool>(A & 0x80);
}

/******************************************************************
  SBC
******************************************************************/
void Cpu::SbcImmediate() { Sbc(Fetch()); }

void Cpu::SbcZeroPage() {
  uint16_t addr = ZeroPage();
  Sbc(ReadMemory(addr));
}

void Cpu::SbcZeroPageX() {
  uint16_t addr = ZeroPageX();
  Sbc(ReadMemory(addr));
}

void Cpu::SbcAbsolute() {
  uint16_t addr = Absolute();
  Sbc(ReadMemory(addr));
}

void Cpu::SbcAbsoluteX() {
  uint16_t addr = AbsoluteX();
  Sbc(ReadMemory(addr));
}

void Cpu::SbcAbsoluteY() {
  uint16_t addr = AbsoluteY();
  Sbc(ReadMemory(addr));
}

void Cpu::SbcIndirectX() {
  uint16_t addr = IndirectX();
  Sbc(ReadMemory(addr));
}

void Cpu::SbcIndirectY() {
  uint16_t addr = IndirectY();
  Sbc(ReadMemory(addr));
}

void Cpu::Sbc(uint8_t value) { Cpu::Adc(value ^ 0xFF); }

/******************************************************************
  AND
******************************************************************/
void Cpu::AndImmediate() { And(Fetch()); }

void Cpu::AndZeroPage() {
  uint16_t addr = ZeroPage();
  And(ReadMemory(addr));
}

void Cpu::AndZeroPageX() {
  uint16_t addr = ZeroPageX();
  And(ReadMemory(addr));
}

void Cpu::AndAbsolute() {
  uint16_t addr = Absolute();
  And(ReadMemory(addr));
}

void Cpu::AndAbsoluteX() {
  uint16_t addr = AbsoluteX();
  And(ReadMemory(addr));
}

void Cpu::AndAbsoluteY() {
  uint16_t addr = AbsoluteY();
  And(ReadMemory(addr));
}

void Cpu::AndIndirectX() {
  uint16_t addr = IndirectX();
  And(ReadMemory(addr));
}

void Cpu::AndIndirectY() {
  uint16_t addr = IndirectY();
  And(ReadMemory(addr));
}

void Cpu::And(uint8_t value) {
  A &= value;
  UpdateNZ(A);
}

/******************************************************************
  EOR
******************************************************************/
void Cpu::EorImmediate() { Eor(Fetch()); }

void Cpu::EorZeroPage() {
  uint16_t addr = ZeroPage();
  Eor(ReadMemory(addr));
}

void Cpu::EorZeroPageX() {
  uint16_t addr = ZeroPageX();
  Eor(ReadMemory(addr));
}

void Cpu::EorAbsolute() {
  uint16_t addr = Absolute();
  Eor(ReadMemory(addr));
}

void Cpu::EorAbsoluteX() {
  uint16_t addr = AbsoluteX();
  Eor(ReadMemory(addr));
}

void Cpu::EorAbsoluteY() {
  uint16_t addr = AbsoluteY();
  Eor(ReadMemory(addr));
}

void Cpu::EorIndirectX() {
  uint16_t addr = IndirectX();
  Eor(ReadMemory(addr));
}

void Cpu::EorIndirectY() {
  uint16_t addr = IndirectY();
  Eor(ReadMemory(addr));
}

void Cpu::Eor(uint8_t value) {
  A ^= value;
  UpdateNZ(A);
}

/******************************************************************
  ORA
******************************************************************/
void Cpu::OraImmediate() { Ora(Fetch()); }

void Cpu::OraZeroPage() {
  uint16_t addr = ZeroPage();
  Ora(ReadMemory(addr));
}

void Cpu::OraZeroPageX() {
  uint16_t addr = ZeroPageX();
  Ora(ReadMemory(addr));
}

void Cpu::OraAbsolute() {
  uint16_t addr = Absolute();
  Ora(ReadMemory(addr));
}

void Cpu::OraAbsoluteX() {
  uint16_t addr = AbsoluteX();
  Ora(ReadMemory(addr));
}

void Cpu::OraAbsoluteY() {
  uint16_t addr = AbsoluteY();
  Ora(ReadMemory(addr));
}

void Cpu::OraIndirectX() {
  uint16_t addr = IndirectX();
  Ora(ReadMemory(addr));
}

void Cpu::OraIndirectY() {
  uint16_t addr = IndirectY();
  Ora(ReadMemory(addr));
}

void Cpu::Ora(uint8_t value) {
  A |= value;
  UpdateNZ(A);
}

/******************************************************************
  ASL
******************************************************************/
void Cpu::AslAccumulator() {
  flag_C = static_cast<bool>(A >> 7);
  A = (A << 1) & 0xFE;
  AddCycle();
  UpdateNZ(A);
}

void Cpu::AslZeroPage() {
  uint16_t addr = ZeroPage();
  Asl(addr, ReadMemory(addr));
}

void Cpu::AslZeroPageX() {
  uint16_t addr = ZeroPageX();
  Asl(addr, ReadMemory(addr));
}

void Cpu::AslAbsolute() {
  uint16_t addr = Absolute();
  Asl(addr, ReadMemory(addr));
}

void Cpu::AslAbsoluteX() {
  uint16_t addr = AbsoluteXW();
  Asl(addr, ReadMemory(addr));
}

void Cpu::Asl(uint16_t addr, uint8_t value) {
  WriteMemory(addr, value);
  flag_C = static_cast<bool>(value >> 7);
  value = (value << 1) & 0xFE;
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/******************************************************************
  LSR
******************************************************************/
void Cpu::LsrAccumulator() {
  flag_C = static_cast<bool>(A & 0x1);
  A = (A >> 1) & 0x7F;
  AddCycle();
  UpdateNZ(A);
}

void Cpu::LsrZeroPage() {
  uint16_t addr = ZeroPage();
  Lsr(addr, ReadMemory(addr));
}

void Cpu::LsrZeroPageX() {
  uint16_t addr = ZeroPageX();
  Lsr(addr, ReadMemory(addr));
}

void Cpu::LsrAbsolute() {
  uint16_t addr = Absolute();
  Lsr(addr, ReadMemory(addr));
}

void Cpu::LsrAbsoluteX() {
  uint16_t addr = AbsoluteXW();
  Lsr(addr, ReadMemory(addr));
}

void Cpu::Lsr(uint16_t addr, uint8_t value) {
  WriteMemory(addr, value);
  flag_C = static_cast<bool>(value & 0x1);
  value = (value >> 1) & 0x7F;
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/******************************************************************
  ROL
******************************************************************/
void Cpu::RolAccumulator() {
  uint8_t old_C = static_cast<uint8_t>(flag_C);
  flag_C = static_cast<bool>(A >> 7);
  A = ((A << 1) & 0xFE) | old_C;
  AddCycle();
  UpdateNZ(A);
}

void Cpu::RolZeroPage() {
  uint16_t addr = ZeroPage();
  Rol(addr, ReadMemory(addr));
}

void Cpu::RolZeroPageX() {
  uint16_t addr = ZeroPageX();
  Rol(addr, ReadMemory(addr));
}

void Cpu::RolAbsolute() {
  uint16_t addr = Absolute();
  Rol(addr, ReadMemory(addr));
}

void Cpu::RolAbsoluteX() {
  uint16_t addr = AbsoluteXW();
  Rol(addr, ReadMemory(addr));
}

void Cpu::Rol(uint16_t addr, uint8_t value) {
  WriteMemory(addr, value);
  uint8_t old_C = static_cast<uint8_t>(flag_C);
  flag_C = static_cast<bool>(value >> 7);
  value = ((value << 1) & 0xFE) | old_C;
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/******************************************************************
  ROR
******************************************************************/
void Cpu::RorAccumulator() {
  uint8_t old_C = static_cast<uint8_t>(flag_C);
  flag_C = static_cast<bool>(A & 0x1);
  A = ((A >> 1) & 0x7F) | (old_C << 7);
  AddCycle();
  UpdateNZ(A);
}

void Cpu::RorZeroPage() {
  uint16_t addr = ZeroPage();
  Ror(addr, ReadMemory(addr));
}

void Cpu::RorZeroPageX() {
  uint16_t addr = ZeroPageX();
  Ror(addr, ReadMemory(addr));
}

void Cpu::RorAbsolute() {
  uint16_t addr = Absolute();
  Ror(addr, ReadMemory(addr));
}

void Cpu::RorAbsoluteX() {
  uint16_t addr = AbsoluteXW();
  Ror(addr, ReadMemory(addr));
}

void Cpu::Ror(uint16_t addr, uint8_t value) {
  WriteMemory(addr, value);
  uint8_t old_C = static_cast<uint8_t>(flag_C);
  flag_C = static_cast<bool>(value & 0x1);
  value = ((value >> 1) & 0x7F) | (old_C << 7);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/*================================================================
  Comparisons
================================================================*/

void Cpu::Cmp(uint8_t reg, uint8_t value) {
  flag_C = reg >= value;
  uint8_t result = reg - value;
  flag_N = static_cast<bool>(result & 0x80);
  flag_Z = result == 0;
}

/******************************************************************
  CMP
******************************************************************/
void Cpu::CmpImmediate() { Cmp(A, Fetch()); }

void Cpu::CmpZeroPage() {
  uint16_t addr = ZeroPage();
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpZeroPageX() {
  uint16_t addr = ZeroPageX();
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpAbsolute() {
  uint16_t addr = Absolute();
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpAbsoluteX() {
  uint16_t addr = AbsoluteX();
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpAbsoluteY() {
  uint16_t addr = AbsoluteY();
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpIndirectX() {
  uint16_t addr = IndirectX();
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpIndirectY() {
  uint16_t addr = IndirectY();
  Cmp(A, ReadMemory(addr));
}

/******************************************************************
  CPX
******************************************************************/
void Cpu::CpxImmediate() { Cmp(X, Fetch()); }

void Cpu::CpxZeroPage() {
  uint16_t addr = ZeroPage();
  Cmp(X, ReadMemory(addr));
}

void Cpu::CpxAbsolute() {
  uint16_t addr = Absolute();
  Cmp(X, ReadMemory(addr));
}

/******************************************************************
  CPY
******************************************************************/
void Cpu::CpyImmediate() { Cmp(Y, Fetch()); }

void Cpu::CpyZeroPage() {
  uint16_t addr = ZeroPage();
  Cmp(Y, ReadMemory(addr));
}

void Cpu::CpyAbsolute() {
  uint16_t addr = Absolute();
  Cmp(Y, ReadMemory(addr));
}

/******************************************************************
  Flag Instructions
******************************************************************/
void Cpu::ClcImplied() { UpdateFlag(flag_C, false); }

void Cpu::CldImplied() { UpdateFlag(flag_D, false); }

void Cpu::CliImplied() { UpdateFlag(flag_I, false); }

void Cpu::ClvImplied() { UpdateFlag(flag_V, false); }

void Cpu::SecImplied() { UpdateFlag(flag_C, true); }

void Cpu::SedImplied() { UpdateFlag(flag_D, true); }

void Cpu::SeiImplied() { UpdateFlag(flag_I, true); }

void Cpu::UpdateFlag(bool& flag, bool value) {
  flag = value;
  AddCycle();
}

/*****************************************************************
   Conditional Branch Instructions
 *****************************************************************/
void Cpu::BccRelative() { Branch(!flag_C); }

void Cpu::BcsRelative() { Branch(flag_C); }

void Cpu::BeqRelative() { Branch(flag_Z); }

void Cpu::BmiRelative() { Branch(flag_N); }

void Cpu::BneRelative() { Branch(!flag_Z); }

void Cpu::BplRelative() { Branch(!flag_N); }

void Cpu::BvcRelative() { Branch(!flag_V); }

void Cpu::BvsRelative() { Branch(flag_V); }

void Cpu::Branch(bool condition) {
  int16_t offset = static_cast<int16_t>(static_cast<int8_t>(Fetch()));
  if (condition) {
    PC = static_cast<uint16_t>(static_cast<int16_t>(PC) + offset);
  }
}

/*****************************************************************
   Jump and Subroutine Instructions
 *****************************************************************/
void Cpu::JmpAbsolute() { PC = Absolute(); }

void Cpu::JmpIndirect() {
  uint16_t addr_lo = Fetch();
  uint16_t addr_hi = Fetch();
  uint16_t addr = (addr_hi << 8) | addr_lo;
  uint16_t lo = ReadMemory(addr);
  uint16_t hi = ReadMemory((addr & 0xFF00) | ((addr + 1) & 0x00FF));
  PC = (hi << 8) | lo;
}

void Cpu::JsrAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  AddCycle();
  Push(static_cast<uint8_t>((PC >> 8) & 0xFF));
  Push(static_cast<uint8_t>(PC & 0xFF));
  PC = (static_cast<uint16_t>(Fetch()) << 8) | lo;
}

void Cpu::RtsImplied() {
  AddCycle();
  SP++;
  AddCycle();
  PC = static_cast<uint16_t>(Pull(SP++)) & 0xFF;
  PC |= static_cast<uint16_t>(Pull(SP)) << 8;
  PC++;
  AddCycle();
}

/*****************************************************************
   Interrupt Instructions
 *****************************************************************/

void Cpu::BrkImplied() {
  Fetch();
  Push(static_cast<uint8_t>(PC >> 8));
  Push(static_cast<uint8_t>(PC));
  uint8_t SR = (static_cast<uint8_t>(flag_N) << 7) |
               (static_cast<uint8_t>(flag_V) << 6) |
               (/*                      */ 1 << 5) |
               (/*                      */ 1 << 4) |
               (static_cast<uint8_t>(flag_D) << 3) |
               (static_cast<uint8_t>(flag_I) << 2) |
               (static_cast<uint8_t>(flag_Z) << 1) |
               (static_cast<uint8_t>(flag_C) << 0);
  Push(SR);
  flag_I = true;
  PC = static_cast<uint16_t>(ReadMemory(0xFFFE));
  PC |= static_cast<uint16_t>(ReadMemory(0xFFFF)) << 8;
}

void Cpu::RtiImplied() {
  AddCycle();
  SP++;
  AddCycle();
  uint8_t status = Pull(SP++);
  flag_N = static_cast<bool>((status >> 7) & 0x1);
  flag_V = static_cast<bool>((status >> 6) & 0x1);
  flag_D = static_cast<bool>((status >> 3) & 0x1);
  flag_I = static_cast<bool>((status >> 2) & 0x1);
  flag_Z = static_cast<bool>((status >> 1) & 0x1);
  flag_C = static_cast<bool>((status >> 0) & 0x1);

  PC = static_cast<uint16_t>(Pull(SP++)) & 0xFF;
  PC |= static_cast<uint16_t>(Pull(SP)) << 8;
}

void Cpu::Interrupt(InterruptType type) {
  AddCycle();
  AddCycle();
  Push(static_cast<uint8_t>((PC >> 8) & 0xFF));
  Push(static_cast<uint8_t>(PC & 0xFF));
  uint8_t SR = (static_cast<uint8_t>(flag_N) << 7) |
               (static_cast<uint8_t>(flag_V) << 6) | (0 << 4) |
               (static_cast<uint8_t>(flag_D) << 3) |
               (static_cast<uint8_t>(flag_I) << 2) |
               (static_cast<uint8_t>(flag_Z) << 1) |
               (static_cast<uint8_t>(flag_C) << 0);
  Push(SR);
  flag_I = true;
  uint16_t addr_lo = type == InterruptType::Irq ? 0xFFFE : 0xFFFA;
  uint16_t addr_hi = type == InterruptType::Irq ? 0xFFFF : 0xFFFB;
  PC = static_cast<uint16_t>(ReadMemory(addr_lo));
  PC |= static_cast<uint16_t>(ReadMemory(addr_hi)) << 8;
}

/*****************************************************************
   BIT
 *****************************************************************/
void Cpu::BitZeroPage() { Bit(static_cast<uint16_t>(Fetch())); }

void Cpu::BitAbsolute() {
  uint16_t addr = Absolute();
  Bit(addr);
}

void Cpu::Bit(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  flag_Z = (A & value) == 0;
  flag_N = static_cast<bool>(value & 0x80);
  flag_V = static_cast<bool>(value & 0x40);
}

/*****************************************************************
   Illegal Opcodes
 *****************************************************************/
void Cpu::AlrImmediate() {
  A &= Fetch();
  flag_C = static_cast<bool>(A & 0x1);
  A = A >> 1;
  UpdateNZ(A);
}

void Cpu::AncImmediate() {
  A &= Fetch();
  flag_C = static_cast<bool>((A >> 7) & 0x1);
  UpdateNZ(A);
}

void Cpu::AneImmediate() {
  A = (A | 0x00) & X & Fetch();
  UpdateNZ(A);
}

void Cpu::ArrImmediate() {
  A = ((static_cast<uint8_t>(flag_C) << 7) | (A & Fetch()) >> 1);
  flag_C = static_cast<bool>((A >> 6) & 0x1);
  flag_V = static_cast<bool>(static_cast<uint8_t>(flag_C) ^ ((A >> 5) & 0x1));
  UpdateNZ(A);
}

void Cpu::DcpZeroPage() { Dcp(ZeroPage()); }

void Cpu::DcpZeroPageX() { Dcp(ZeroPageX()); }

void Cpu::DcpAbsolute() { Dcp(Absolute()); }

void Cpu::DcpAbsoluteX() { Dcp(AbsoluteXW()); }

void Cpu::DcpAbsoluteY() { Dcp(AbsoluteYW()); }

void Cpu::DcpIndirectX() { Dcp(IndirectX()); }

void Cpu::DcpIndirectY() { Dcp(IndirectYW()); }

void Cpu::Dcp(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  WriteMemory(addr, value);
  Cmp(A, value - 1);
  WriteMemory(addr, value - 1);
}

void Cpu::IscZeroPage() { Isc(ZeroPage()); }

void Cpu::IscZeroPageX() { Isc(ZeroPageX()); }

void Cpu::IscAbsolute() { Isc(Absolute()); }

void Cpu::IscAbsoluteX() { Isc(AbsoluteX()); }

void Cpu::IscAbsoluteY() { Isc(AbsoluteY()); }

void Cpu::IscIndirectX() { Isc(IndirectX()); }

void Cpu::IscIndirectY() { Isc(IndirectYW()); }

void Cpu::Isc(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  WriteMemory(addr, value);
  Sbc(value + 1);
  WriteMemory(addr, value + 1);
}

void Cpu::LasAbsoluteY() {
  uint8_t value = SP & ReadMemory(AbsoluteY());
  A = value;
  X = value;
  SP = value;
  UpdateNZ(value);
}

void Cpu::LaxZeroPage() { Lax(ZeroPage()); }

void Cpu::LaxZeroPageY() { Lax(ZeroPageY()); }

void Cpu::LaxAbsolute() { Lax(Absolute()); }

void Cpu::LaxAbsoluteY() { Lax(AbsoluteY()); }

void Cpu::LaxIndirectX() { Lax(IndirectX()); }

void Cpu::LaxIndirectY() { Lax(IndirectYW()); }

void Cpu::Lax(uint16_t addr) {
  A = ReadMemory(addr);
  X = A;
  UpdateNZ(A);
}

void Cpu::LxaImmediate() {
  uint8_t value = Fetch();
  A = value & 0xFF;
  X = A;
  UpdateNZ(A);
}

void Cpu::RlaZeroPage() { Rla(ZeroPage()); }

void Cpu::RlaZeroPageX() { Rla(ZeroPageX()); }

void Cpu::RlaAbsolute() { Rla(Absolute()); }

void Cpu::RlaAbsoluteX() { Rla(AbsoluteXW()); }

void Cpu::RlaAbsoluteY() { Rla(AbsoluteYW()); }

void Cpu::RlaIndirectX() { Rla(IndirectX()); }

void Cpu::RlaIndirectY() { Rla(IndirectYW()); }

void Cpu::Rla(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  WriteMemory(addr, value);
  bool carry = static_cast<uint8_t>(flag_C);
  flag_C = static_cast<bool>((value >> 7) & 0x1);
  value = (value << 1) | carry;
  A &= value;
  UpdateNZ(A);
  WriteMemory(addr, value);
}

void Cpu::RraZeroPage() { Rra(ZeroPage()); }

void Cpu::RraZeroPageX() { Rra(ZeroPageX()); }

void Cpu::RraAbsolute() { Rra(Absolute()); }

void Cpu::RraAbsoluteX() { Rra(AbsoluteXW()); }

void Cpu::RraAbsoluteY() { Rra(AbsoluteYW()); }

void Cpu::RraIndirectX() { Rra(IndirectX()); }

void Cpu::RraIndirectY() { Rra(IndirectYW()); }

void Cpu::Rra(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  WriteMemory(addr, value);
  bool carry = static_cast<uint8_t>(flag_C);
  flag_C = static_cast<bool>(value & 0x1);
  value = (carry << 7) | (value >> 1);
  uint16_t result = static_cast<uint16_t>(A) + static_cast<uint16_t>(value) +
                    static_cast<uint16_t>(flag_C);
  flag_V = static_cast<bool>(
      ~(static_cast<uint16_t>(A) ^ static_cast<uint16_t>(value)) &
      (static_cast<uint16_t>(A) ^ static_cast<uint16_t>(result)) & 0x80);
  flag_C = static_cast<bool>(result & 0x100);
  A = static_cast<uint8_t>(result & 0xFF);
  UpdateNZ(A);
  WriteMemory(addr, value);
}

void Cpu::SaxZeroPage() { Sax(ZeroPage()); }

void Cpu::SaxZeroPageY() { Sax(ZeroPageY()); }

void Cpu::SaxAbsolute() { Sax(Absolute()); }

void Cpu::SaxIndirectX() { Sax(IndirectX()); }

void Cpu::Sax(uint16_t addr) { WriteMemory(addr, A & X); }

void Cpu::SbxImmediate() {
  int16_t result = (static_cast<int16_t>(A) & static_cast<int16_t>(X)) -
                   static_cast<int16_t>(Fetch());
  flag_C = static_cast<bool>(result >= 0);
  flag_Z = result == 0;
  X = static_cast<uint8_t>(result & 0xFF);
  flag_N = static_cast<bool>((result >> 7) & 0x1);
}

void Cpu::ShaAbsoluteY() { Sha(AbsoluteYW()); }

void Cpu::ShaIndirectY() { Sha(IndirectYW()); }

void Cpu::Sha(uint16_t addr) {
  WriteMemory(addr, A & X & (static_cast<uint8_t>(addr >> 8) + 1));
}

void Cpu::ShxAbsoluteY() {
  uint8_t lo = Fetch();
  uint8_t hi = Fetch();
  bool inc = static_cast<uint16_t>(lo) + static_cast<uint16_t>(Y) > 0xFF;
  lo += Y;
  ReadMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo));
  if (inc) {
    hi &= X;
  }
  WriteMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo),
              X & (hi + 1));
}

void Cpu::ShyAbsoluteX() {
  uint8_t lo = Fetch();
  uint8_t hi = Fetch();
  bool inc = (static_cast<uint16_t>(lo) + X) > 0xFF;
  lo += X;
  ReadMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo));
  if (inc) {
    hi &= Y;
  }
  WriteMemory((static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo),
              Y & (hi + 1));
}

void Cpu::SloZeroPage() { Slo(ZeroPage()); }

void Cpu::SloZeroPageX() { Slo(ZeroPageX()); }

void Cpu::SloAbsolute() { Slo(Absolute()); }

void Cpu::SloAbsoluteX() { Slo(AbsoluteXW()); }

void Cpu::SloAbsoluteY() { Slo(AbsoluteYW()); }

void Cpu::SloIndirectX() { Slo(IndirectX()); }

void Cpu::SloIndirectY() { Slo(IndirectYW()); }

void Cpu::Slo(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  WriteMemory(addr, value);
  flag_C = static_cast<bool>((value >> 7) & 0x1);
  value <<= 1;
  A |= value;
  UpdateNZ(A);
  WriteMemory(addr, value);
}

void Cpu::SreZeroPage() { Sre(ZeroPage()); }

void Cpu::SreZeroPageX() { Sre(ZeroPageX()); }

void Cpu::SreAbsolute() { Sre(Absolute()); }

void Cpu::SreAbsoluteX() { Sre(AbsoluteXW()); }

void Cpu::SreAbsoluteY() { Sre(AbsoluteY()); }

void Cpu::SreIndirectX() { Sre(IndirectX()); }

void Cpu::SreIndirectY() { Sre(IndirectYW()); }

void Cpu::Sre(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  WriteMemory(addr, value);
  flag_C = static_cast<bool>(value & 0x1);
  value >>= 1;
  A ^= value;
  UpdateNZ(A);
  WriteMemory(addr, value);
}

void Cpu::TasAbsoluteY() {
  uint16_t addr = AbsoluteY();
  SP = A & X;
  WriteMemory(addr, A & X & (static_cast<uint8_t>(addr >> 8) + 1));
}

void Cpu::NopImplied() { AddCycle(); }

void Cpu::NopImmediate() { Fetch(); }

void Cpu::NopZeroPage() { ZeroPage(); }

void Cpu::NopZeroPageX() { ZeroPageX(); }

void Cpu::NopAbsolute() { Absolute(); }

void Cpu::NopAbsoluteX() { AbsoluteX(); }

void Cpu::Stp() { stopped = true; }

/*****************************************************************
   Utility
 *****************************************************************/

bool Cpu::NmiPending() { return mmu.NmiPending(); }

bool Cpu::IrqPending() { return mmu.IrqPending(); }

void Cpu::Push(uint8_t value) {
  WriteMemory(0x0100 | static_cast<uint16_t>(SP--), value);
}

uint8_t Cpu::Pull(uint8_t SP) {
  return ReadMemory(0x0100 | static_cast<uint16_t>(SP));
}

void Cpu::UpdateNZV(uint8_t old, uint8_t byte) {
  UpdateNZ(byte);
  bool old7 = static_cast<bool>(old >> 7);
  bool new7 = static_cast<bool>(byte >> 7);
  flag_V = (!old7 && !new7 && flag_C) || (old7 && new7 && !flag_C);
}

void Cpu::UpdateNZ(uint8_t byte) {
  flag_N = static_cast<bool>(byte >> 7);
  flag_Z = byte == 0;
}

uint8_t Cpu::ReadMemory(uint16_t addr) {
  AddCycle();
  return mmu.Read(addr);
}

void Cpu::WriteMemory(uint16_t addr, uint8_t value) {
  AddCycle();
  mmu.Write(addr, value);
}

uint8_t Cpu::Fetch() {
  AddCycle();
  return mmu.Read(PC++);
}

void Cpu::AddCycle() {
  if (mmu.StallCpu()) {
    uint64_t n = mmu.InDma() ? 2 : 4;
    cycles += n;
    event_cycles += n;
    mmu.PpuTick(3 * n);
    mmu.ApuTick(n);
    return;
  }

  cycles++;
  event_cycles++;
  mmu.PpuTick(3);
  mmu.ApuTick(1);
}

}  // namespace cpu
