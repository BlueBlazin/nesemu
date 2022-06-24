#include "cpu.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "src/memory/memory.h"

namespace cpu {

std::string to_hex(uint16_t PC) {
  std::ostringstream ss;
  ss << std::hex << PC;
  return ss.str();
}

Cpu::Cpu(const std::string& path) : mmu(path) { myfile.open("emu.log"); }

void Cpu::Startup() {
  // PC = static_cast<uint16_t>(mmu.Read(0xFFFC));
  // PC |= static_cast<uint16_t>(mmu.Read(0xFFFD)) << 8;
  // std::cout << "initial PC: " << std::hex << PC << std::endl;
  PC = 0x0C000;
}

void Cpu::Run() {
  /* fetch-decode-execute loop */
  while (true) {
    Tick();
  }
}

void Cpu::Tick() {
  myfile << to_hex(PC) << std::endl;

  if (mmu.InDma()) {
    std::cout << "in dma" << std::endl;
    RunDma();
  }

  // std::cout << "PC: 0x" << std::hex << PC << std::endl;
  // std::printf("0x02: %X, 0x03: %X\n", mmu.Read(0x02), mmu.Read(0x03));
  DecodeExecute(Fetch());

  // Check for interrupts
  if (NmiPending()) {
    std::cout << "NMI" << std::endl;
    mmu.ClearNmi();
    Interrupt(InterruptType::Nmi);
  }
}

uint8_t* Cpu::GetScreen() { return mmu.GetScreen(); }

void Cpu::RunDma() {
  if (dma_state == DmaState::PreDma) {
    dma_state = cycles % 2 == 1 ? DmaState::OddCycleWait : DmaState::Running;
    AddCycles(1);
  } else if (dma_state == DmaState::OddCycleWait) {
    dma_state = DmaState::Running;
    AddCycles(1);
  } else {
    mmu.DmaTick();
  }
}

void Cpu::DecodeExecute(uint8_t opcode) {
  switch (opcode) {
    case 0x00:
      BrkImplied();
      break;
    case 0x01:
      OraIndirectX();
      break;
    case 0x05:
      OraZeroPage();
      break;
    case 0x06:
      AslZeroPage();
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
    case 0x0D:
      OraAbsolute();
      break;
    case 0x0E:
      AslAbsolute();
      break;
    case 0x10:
      BplRelative();
      break;
    case 0x11:
      OraIndirectY();
      break;
    case 0x15:
      OraZeroPageX();
      break;
    case 0x16:
      AslZeroPageX();
      break;
    case 0x18:
      ClcImplied();
      break;
    case 0x19:
      OraAbsoluteY();
      break;
    case 0x1D:
      OraAbsoluteX();
      break;
    case 0x1E:
      AslAbsoluteX();
      break;
    case 0x20:
      JsrAbsolute();
      break;
    case 0x21:
      AndIndirectX();
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
    case 0x28:
      PlpImplied();
      break;
    case 0x29:
      AndImmediate();
      break;
    case 0x2A:
      RolAccumulator();
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
    case 0x30:
      BmiRelative();
      break;
    case 0x31:
      AndIndirectY();
      break;
    case 0x35:
      AndZeroPageX();
      break;
    case 0x36:
      RolZeroPageX();
      break;
    case 0x38:
      SecImplied();
      break;
    case 0x39:
      AndAbsoluteY();
      break;
    case 0x3D:
      AndAbsoluteX();
      break;
    case 0x3E:
      RolAbsoluteX();
      break;
    case 0x40:
      RtiImplied();
      break;
    case 0x41:
      EorIndirectX();
      break;
    case 0x45:
      EorZeroPage();
      break;
    case 0x46:
      LsrZeroPage();
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
    case 0x4C:
      JmpAbsolute();
      break;
    case 0x4D:
      EorAbsolute();
      break;
    case 0x4E:
      LsrAbsolute();
      break;
    case 0x50:
      BvcRelative();
      break;
    case 0x51:
      EorIndirectY();
      break;
    case 0x55:
      EorZeroPageX();
      break;
    case 0x56:
      LsrZeroPageX();
      break;
    case 0x58:
      CliImplied();
      break;
    case 0x59:
      EorAbsoluteY();
      break;
    case 0x5D:
      EorAbsoluteX();
      break;
    case 0x5E:
      LsrAbsoluteX();
      break;
    case 0x60:
      RtsImplied();
      break;
    case 0x61:
      AdcIndirectX();
      break;
    case 0x65:
      AdcZeroPage();
      break;
    case 0x66:
      RorZeroPage();
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
    case 0x6C:
      JmpIndirect();
      break;
    case 0x6D:
      AdcAbsolute();
      break;
    case 0x6E:
      RorAbsolute();
      break;
    case 0x70:
      BvsRelative();
      break;
    case 0x71:
      AdcIndirectY();
      break;
    case 0x75:
      AdcZeroPageX();
      break;
    case 0x76:
      RorZeroPageX();
      break;
    case 0x78:
      SeiImplied();
      break;
    case 0x79:
      AdcAbsoluteY();
      break;
    case 0x7D:
      AdcAbsoluteX();
      break;
    case 0x7E:
      RorAbsoluteX();
      break;
    case 0x81:
      StaIndirectX();
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
    case 0x88:
      DeyImplied();
      break;
    case 0x8A:
      TxaImplied();
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
    case 0x90:
      BccRelative();
      break;
    case 0x91:
      StaIndirectY();
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
    case 0x98:
      TyaImplied();
      break;
    case 0x99:
      StaAbsoluteY();
      break;
    case 0x9A:
      TxsImplied();
      break;
    case 0x9D:
      StaAbsoluteX();
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
    case 0xA4:
      LdyZeroPage();
      break;
    case 0xA5:
      LdaZeroPage();
      break;
    case 0xA6:
      LdxZeroPage();
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
    case 0xAC:
      LdyAbsolute();
      break;
    case 0xAD:
      LdaAbsolute();
      break;
    case 0xAE:
      LdxAbsolute();
      break;
    case 0xB0:
      BcsRelative();
      break;
    case 0xB1:
      LdaIndirectY();
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
    case 0xB8:
      ClvImplied();
      break;
    case 0xB9:
      LdaAbsoluteY();
      break;
    case 0xBA:
      TsxImplied();
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
    case 0xC0:
      CpyImmediate();
      break;
    case 0xC1:
      CmpIndirectX();
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
    case 0xC8:
      InyImplied();
      break;
    case 0xC9:
      CmpImmediate();
      break;
    case 0xCA:
      DexImplied();
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
    case 0xD0:
      BneRelative();
      break;
    case 0xD1:
      CmpIndirectY();
      break;
    case 0xD5:
      CmpZeroPageX();
      break;
    case 0xD6:
      DecZeroPageX();
      break;
    case 0xD8:
      CldImplied();
      break;
    case 0xD9:
      CmpAbsoluteY();
      break;
    case 0xDD:
      CmpAbsoluteX();
      break;
    case 0xDE:
      DecAbsoluteX();
      break;
    case 0xE0:
      CpxImmediate();
      break;
    case 0xE1:
      SbcIndirectX();
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
    case 0xE8:
      InxImplied();
      break;
    case 0xE9:
      SbcImmediate();
      break;
    case 0xEA:
      AddCycles(1);
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
    case 0xF0:
      BeqRelative();
      break;
    case 0xF1:
      SbcIndirectY();
      break;
    case 0xF5:
      SbcZeroPageX();
      break;
    case 0xF6:
      IncZeroPageX();
      break;
    case 0xF8:
      SedImplied();
      break;
    case 0xF9:
      SbcAbsoluteY();
      break;
    case 0xFD:
      SbcAbsoluteX();
      break;
    case 0xFE:
      IncAbsoluteX();
      break;
  }
}

/*=================================================================
*  Instructions
=================================================================*/

// uint16_t Cpu::IndirectX() {
//   uint16_t ptr_addr = static_cast<uint16_t>(Fetch());
//   uint16_t ptr = static_cast<uint16_t>(ReadMemory(ptr_addr));
//   uint16_t lo =
//       static_cast<uint16_t>(ReadMemory(ptr + static_cast<uint16_t>(X)));
//   uint16_t hi =
//       static_cast<uint16_t>(ReadMemory(ptr + static_cast<uint16_t>(X) + 1));
//   return (hi << 8) | lo;
// }
uint16_t Cpu::IndirectX() {
  uint16_t ptr = static_cast<uint16_t>(Fetch());
  ReadMemory(ptr);
  uint16_t lo_addr = ptr + static_cast<uint16_t>(X);
  uint16_t hi_addr = ptr + static_cast<uint16_t>(X) + 1;
  uint16_t lo = static_cast<uint16_t>(ReadMemory(lo_addr & 0xFF));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(hi_addr & 0xFF));
  return (hi << 8) | lo;
}

uint16_t Cpu::IndirectY() {
  uint16_t ptr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(ptr & 0xFF));
  uint16_t hi = static_cast<uint16_t>(ReadMemory((ptr + 1) & 0xFF));
  return ((hi << 8) | lo) + static_cast<uint16_t>(Y);
}

uint16_t Cpu::ZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  return ReadMemory(addr) + static_cast<uint16_t>(X);
}

uint16_t Cpu::ZeroPageY() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  return ReadMemory(addr) + static_cast<uint16_t>(Y);
}

/******************************************************************
  LDA
******************************************************************/
void Cpu::LdaImmediate() {
  A = Fetch();
  UpdateNZ(A);
}

void Cpu::LdaZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaZeroPageX() {
  uint16_t addr = ZeroPageX();
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  A = ReadMemory((hi << 8) | lo);
  if (PC == 0xCE1E + 3) {
    std::printf("Addr: %X, LDA A: %X\n", (hi << 8) | lo, A);
  }
  UpdateNZ(A);
}

void Cpu::LdaAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(X);
  if (addr > 0xFF) {
    AddCycles(1);
  }
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(Y);
  if (addr > 0xFF) {
    AddCycles(1);
  }
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaIndirectX() {
  // uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  // AddCycles(1);
  // uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  // uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  // uint16_t addr = (hi << 8) | lo;
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

void Cpu::LdxZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  X = ReadMemory(addr);
  UpdateNZ(X);
}

void Cpu::LdxZeroPageY() {
  uint16_t addr = ZeroPageY();
  X = ReadMemory(addr);
  UpdateNZ(X);
}

void Cpu::LdxAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  X = ReadMemory((hi << 8) | lo);
  UpdateNZ(X);
}

void Cpu::LdxAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(Y);
  if (addr > 0xFF) {
    AddCycles(1);
  }
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

void Cpu::LdyZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Y = ReadMemory(addr);
  UpdateNZ(Y);
}

void Cpu::LdyZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  Y = ReadMemory(addr);
  UpdateNZ(Y);
}

void Cpu::LdyAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  Y = ReadMemory((hi << 8) | lo);
  UpdateNZ(Y);
}

void Cpu::LdyAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(X);
  if (addr > 0xFF) {
    AddCycles(1);
  }
  Y = ReadMemory(addr);
  UpdateNZ(Y);
}

/******************************************************************
  STA
******************************************************************/
void Cpu::StaZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  WriteMemory(addr, A);
}

void Cpu::StaZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  WriteMemory(addr, A);
}

void Cpu::StaAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  WriteMemory((hi << 8) | lo, A);
}

void Cpu::StaAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(X);
  AddCycles(1);
  WriteMemory(addr, A);
}

void Cpu::StaAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(Y);
  AddCycles(1);
  WriteMemory(addr, A);
}

void Cpu::StaIndirectX() {
  // uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  // AddCycles(1);
  // uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  // uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  // uint16_t addr = (hi << 8) | lo;
  uint16_t addr = IndirectX();
  WriteMemory(addr, A);
}

void Cpu::StaIndirectY() {
  uint16_t addr = IndirectY();
  ReadMemory(addr);
  WriteMemory(addr, A);
}

/******************************************************************
  STX
******************************************************************/
void Cpu::StxZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  WriteMemory(addr, X);
}

void Cpu::StxZeroPageY() {
  uint16_t addr = static_cast<uint16_t>(Fetch() + Y);
  AddCycles(1);
  WriteMemory(addr, X);
}

void Cpu::StxAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  WriteMemory((hi << 8) | lo, X);
}

/******************************************************************
  STY
******************************************************************/
void Cpu::StyZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  WriteMemory(addr, Y);
}

void Cpu::StyZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  WriteMemory(addr, Y);
}

void Cpu::StyAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  WriteMemory((hi << 8) | lo, Y);
}

/******************************************************************
  TAX
******************************************************************/
void Cpu::TaxImplied() {
  X = A;
  AddCycles(1);
  UpdateNZ(X);
}

/******************************************************************
  TAY
******************************************************************/
void Cpu::TayImplied() {
  Y = A;
  AddCycles(1);
  UpdateNZ(Y);
}

/******************************************************************
  TSX
******************************************************************/
void Cpu::TsxImplied() {
  X = SP;
  AddCycles(1);
  UpdateNZ(X);
}

/******************************************************************
  TXA
******************************************************************/
void Cpu::TxaImplied() {
  A = X;
  AddCycles(1);
  UpdateNZ(A);
}

/******************************************************************
  TSX
******************************************************************/
void Cpu::TxsImplied() {
  SP = X;
  AddCycles(1);
}

/******************************************************************
  TYA
******************************************************************/
void Cpu::TyaImplied() {
  A = Y;
  AddCycles(1);
  UpdateNZ(A);
}

/******************************************************************
  PHA
******************************************************************/
void Cpu::PhaImplied() {
  AddCycles(1);
  Push(A);
}

/******************************************************************
  PHP
******************************************************************/
void Cpu::PhpImplied() {
  AddCycles(1);
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
  AddCycles(1);
  SP++;
  AddCycles(1);
  A = Pull(SP);
  UpdateNZ(A);
}

/******************************************************************
  PLP
******************************************************************/
void Cpu::PlpImplied() {
  AddCycles(1);
  SP++;
  AddCycles(1);
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
  uint16_t addr = static_cast<uint16_t>(Fetch());
  uint8_t value = ReadMemory(addr) - 1;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

void Cpu::DecZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint8_t value = ReadMemory(addr) - 1;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

void Cpu::DecAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  uint8_t value = ReadMemory(addr) - 1;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

void Cpu::DecAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + X;
  AddCycles(1);
  uint8_t value = ReadMemory(addr) - 1;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/******************************************************************
  DEX
******************************************************************/
void Cpu::DexImplied() {
  X--;
  AddCycles(1);
  UpdateNZ(X);
}

/******************************************************************
  DEY
******************************************************************/
void Cpu::DeyImplied() {
  Y--;
  AddCycles(1);
  UpdateNZ(Y);
}

/******************************************************************
  INC
******************************************************************/
void Cpu::IncZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  uint8_t value = ReadMemory(addr) + 1;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

void Cpu::IncZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint8_t value = ReadMemory(addr) + 1;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

void Cpu::IncAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  uint8_t value = ReadMemory(addr) + 1;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

void Cpu::IncAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + X;
  AddCycles(1);
  uint8_t value = ReadMemory(addr) + 1;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/******************************************************************
  INX
******************************************************************/
void Cpu::InxImplied() {
  X++;
  AddCycles(1);
  UpdateNZ(X);
}

/******************************************************************
  INY
******************************************************************/
void Cpu::InyImplied() {
  Y++;
  AddCycles(1);
  UpdateNZ(Y);
}

/******************************************************************
  ADC
******************************************************************/
void Cpu::AdcImmediate() { Adc(Fetch()); }

void Cpu::AdcZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Adc(ReadMemory(addr));
}

void Cpu::AdcZeroPageX() {
  uint16_t addr = ZeroPageX();
  Adc(ReadMemory(addr));
}

void Cpu::AdcAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Adc(ReadMemory(addr));
}

void Cpu::AdcAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(X))) / 256) {
    AddCycles(1);
  }
  Adc(ReadMemory(addr + static_cast<uint16_t>(X)));
}

void Cpu::AdcAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(Y))) / 256) {
    AddCycles(1);
  }
  Adc(ReadMemory(addr + static_cast<uint16_t>(Y)));
}

void Cpu::AdcIndirectX() {
  // uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  // AddCycles(1);
  // uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  // uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  // uint16_t addr = (hi << 8) | lo;
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
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Sbc(ReadMemory(addr));
}

void Cpu::SbcZeroPageX() {
  uint16_t addr = ZeroPageX();
  Sbc(ReadMemory(addr));
}

void Cpu::SbcAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Sbc(ReadMemory(addr));
}

void Cpu::SbcAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(X))) / 256) {
    AddCycles(1);
  }
  Sbc(ReadMemory(addr + static_cast<uint16_t>(X)));
}

void Cpu::SbcAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(Y))) / 256) {
    AddCycles(1);
  }
  Sbc(ReadMemory(addr + static_cast<uint16_t>(Y)));
}

void Cpu::SbcIndirectX() {
  // uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  // AddCycles(1);
  // uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  // uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  // uint16_t addr = (hi << 8) | lo;
  uint16_t addr = IndirectX();
  Sbc(ReadMemory(addr));
}

void Cpu::SbcIndirectY() {
  uint16_t addr = IndirectY();
  Sbc(ReadMemory(addr));
}

void Cpu::Sbc(uint8_t value) {
  // flag_C = true;
  Cpu::Adc(value ^ 0xFF);
}

/******************************************************************
  AND
******************************************************************/
void Cpu::AndImmediate() { And(Fetch()); }

void Cpu::AndZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  And(ReadMemory(addr));
}

void Cpu::AndZeroPageX() {
  uint16_t addr = ZeroPageX();
  And(ReadMemory(addr));
}

void Cpu::AndAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  And(ReadMemory(addr));
}

void Cpu::AndAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(X))) / 256) {
    AddCycles(1);
  }
  And(ReadMemory(addr + static_cast<uint16_t>(X)));
}

void Cpu::AndAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(Y))) / 256) {
    AddCycles(1);
  }
  And(ReadMemory(addr + static_cast<uint16_t>(Y)));
}

void Cpu::AndIndirectX() {
  // uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  // AddCycles(1);
  // uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  // uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  // uint16_t addr = (hi << 8) | lo;
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
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Eor(ReadMemory(addr));
}

void Cpu::EorZeroPageX() {
  uint16_t addr = ZeroPageX();
  Eor(ReadMemory(addr));
}

void Cpu::EorAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Eor(ReadMemory(addr));
}

void Cpu::EorAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(X))) / 256) {
    AddCycles(1);
  }
  Eor(ReadMemory(addr + static_cast<uint16_t>(X)));
}

void Cpu::EorAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(Y))) / 256) {
    AddCycles(1);
  }
  Eor(ReadMemory(addr + static_cast<uint16_t>(Y)));
}

void Cpu::EorIndirectX() {
  // uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  // AddCycles(1);
  // uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  // uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  // uint16_t addr = (hi << 8) | lo;
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
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Ora(ReadMemory(addr));
}

void Cpu::OraZeroPageX() {
  uint16_t addr = ZeroPageX();
  Ora(ReadMemory(addr));
}

void Cpu::OraAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Ora(ReadMemory(addr));
}

void Cpu::OraAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(X))) / 256) {
    AddCycles(1);
  }
  Ora(ReadMemory(addr + static_cast<uint16_t>(X)));
}

void Cpu::OraAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(Y))) / 256) {
    AddCycles(1);
  }
  Ora(ReadMemory(addr + static_cast<uint16_t>(Y)));
}

void Cpu::OraIndirectX() {
  // uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  // AddCycles(1);
  // uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  // uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  // uint16_t addr = (hi << 8) | lo;
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
  AddCycles(1);
  UpdateNZ(A);
}

void Cpu::AslZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Asl(addr);
}

void Cpu::AslZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
  Asl(addr);
}

void Cpu::AslAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Asl(addr);
}

void Cpu::AslAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  AddCycles(1);
  Asl(addr + static_cast<uint16_t>(X));
}

void Cpu::Asl(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  flag_C = static_cast<bool>(value >> 7);
  value = (value << 1) & 0xFE;
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/******************************************************************
  LSR
******************************************************************/
void Cpu::LsrAccumulator() {
  flag_C = static_cast<bool>(A & 0x1);
  A = (A >> 1) & 0x7F;
  AddCycles(1);
  UpdateNZ(A);
}

void Cpu::LsrZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Lsr(addr);
}

void Cpu::LsrZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
  Lsr(addr);
}

void Cpu::LsrAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Lsr(addr);
}

void Cpu::LsrAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  AddCycles(1);
  Lsr(addr + static_cast<uint16_t>(X));
}

void Cpu::Lsr(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  flag_C = static_cast<bool>(value & 0x1);
  value = (value >> 1) & 0x7F;
  AddCycles(1);
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
  AddCycles(1);
  UpdateNZ(A);
}

void Cpu::RolZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Rol(addr);
}

void Cpu::RolZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
  Rol(addr);
}

void Cpu::RolAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Rol(addr);
}

void Cpu::RolAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  AddCycles(1);
  Rol(addr + static_cast<uint16_t>(X));
}

void Cpu::Rol(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  uint8_t old_C = static_cast<uint8_t>(flag_C);
  flag_C = static_cast<bool>(A >> 7);
  value = ((value << 1) & 0xFE) | old_C;
  AddCycles(1);
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
  AddCycles(1);
  UpdateNZ(A);
}

void Cpu::RorZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Ror(addr);
}

void Cpu::RorZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
  Ror(addr);
}

void Cpu::RorAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Ror(addr);
}

void Cpu::RorAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  AddCycles(1);
  Ror(addr + static_cast<uint16_t>(X));
}

void Cpu::Ror(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  uint8_t old_C = static_cast<uint8_t>(flag_C);
  flag_C = static_cast<bool>(A & 0x1);
  value = ((value >> 1) & 0x7F) | (old_C << 7);
  AddCycles(1);
  WriteMemory(addr, value);
  UpdateNZ(value);
}

/*================================================================
  Comparisons
================================================================*/
// void Cpu::Cmp(uint8_t reg, uint8_t value) {
//   uint8_t old = reg;
//   reg += value;
//   flag_C = reg < old;
//   reg += static_cast<uint16_t>(flag_C);
//   flag_C = flag_C || (reg < old);
//   UpdateNZ(reg);
// }

void Cpu::Cmp(uint8_t reg, uint8_t value) {
  if (PC == 0xCE21 + 2) {
    // std::cout << "A: " << (unsigned int)A << std::endl;
    std::printf("A: %X\n", A);
  }
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
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpZeroPageX() {
  uint16_t addr = ZeroPageX();
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Cmp(A, ReadMemory(addr));
}

void Cpu::CmpAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(X))) / 256) {
    AddCycles(1);
  }
  Cmp(A, ReadMemory(addr + static_cast<uint16_t>(X)));
}

void Cpu::CmpAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(Y))) / 256) {
    AddCycles(1);
  }
  Cmp(A, ReadMemory(addr + static_cast<uint16_t>(Y)));
}

void Cpu::CmpIndirectX() {
  // uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  // AddCycles(1);
  // uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  // uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  // uint16_t addr = (hi << 8) | lo;
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
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Cmp(X, ReadMemory(addr));
}

void Cpu::CpxAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Cmp(X, ReadMemory(addr));
}

/******************************************************************
  CPY
******************************************************************/
void Cpu::CpyImmediate() { Cmp(Y, Fetch()); }

void Cpu::CpyZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Cmp(Y, ReadMemory(addr));
}

void Cpu::CpyAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
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
  AddCycles(1);
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
void Cpu::JmpAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  PC = (hi << 8) | lo;
}

// void Cpu::JmpIndirect() {
//   uint16_t addr_lo = Fetch();
//   uint16_t addr_hi = Fetch();
//   uint16_t latch = static_cast<uint16_t>(ReadMemory(addr_lo));
//   PC = static_cast<uint16_t>(ReadMemory(addr_hi)) << 8;
//   PC |= latch;
// }

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
  AddCycles(1);
  Push(static_cast<uint8_t>((PC >> 8) & 0xFF));
  Push(static_cast<uint8_t>(PC & 0xFF));
  // PC = lo;
  PC = (static_cast<uint16_t>(Fetch()) << 8) | lo;
}

void Cpu::RtsImplied() {
  AddCycles(1);
  SP++;
  AddCycles(1);
  PC = static_cast<uint16_t>(Pull(SP++)) & 0xFF;
  PC |= static_cast<uint16_t>(Pull(SP)) << 8;
  PC++;
  AddCycles(1);
}

/*****************************************************************
   Interrupt Instructions
 *****************************************************************/

void Cpu::BrkImplied() {
  Fetch();
  uint8_t lo = static_cast<uint8_t>(PC & 0xFF);
  uint8_t hi = static_cast<uint8_t>((PC >> 8) & 0xFF);
  // flag_B = true;
  Push(hi);
  Push(lo);
  uint8_t SR = (static_cast<uint8_t>(flag_N) << 7) |
               (static_cast<uint8_t>(flag_V) << 6) |
               (/*static_cast<uint8_t>(flag_B)*/ 1 << 4) |
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
  AddCycles(1);
  SP++;
  AddCycles(1);
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
  AddCycles(2);
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
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  Bit((hi << 8) | lo);
}

void Cpu::Bit(uint16_t addr) {
  uint8_t value = ReadMemory(addr);
  flag_Z = (A & value) == 0;
  flag_N = static_cast<bool>(value & 0x80);
  flag_V = static_cast<bool>(value & 0x40);
}

/*****************************************************************
   Utility
 *****************************************************************/

bool Cpu::NmiPending() { return mmu.NmiPending(); }

bool Cpu::IrqPending() {}

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
  AddCycles(1);
  return mmu.Read(addr);
}

void Cpu::WriteMemory(uint16_t addr, uint8_t value) {
  AddCycles(1);
  mmu.Write(addr, value);
}

uint8_t Cpu::Fetch() {
  AddCycles(1);
  return mmu.Read(PC++);
}

void Cpu::AddCycles(uint64_t n) {
  cycles += n;
  mmu.PpuTick(3 * n);
}

}  // namespace cpu