#include "cpu.h"

#include <cstdint>

namespace cpu {

Cpu::Cpu() : mmu() {}

void Cpu::Run() {
  /* fetch-decode */
  while (true) {
    switch (Fetch()) {
      // TODO
    }
  }
}

/*****************************************************************
 *  Instructions
 *****************************************************************/

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
  uint16_t addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  A = ReadMemory((hi << 8) | lo);
  UpdateNZ(A);
}

void Cpu::LdaAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(X) +
                  static_cast<uint16_t>(flag_C);
  if (addr > 0xFF) {
    AddCycles(1);
  }
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(Y) +
                  static_cast<uint16_t>(flag_C);
  if (addr > 0xFF) {
    AddCycles(1);
  }
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaIndirectX() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = (hi << 8) | lo;
  A = ReadMemory(addr);
  UpdateNZ(A);
}

void Cpu::LdaIndirectY() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(Y);
  if ((addr + static_cast<uint16_t>(flag_C)) / 256 > addr / 256) {
    AddCycles(1);
  }
  A = ReadMemory(addr + static_cast<uint16_t>(flag_C));
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
  uint16_t addr = static_cast<uint16_t>(Fetch() + Y);
  AddCycles(1);
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
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(Y) +
                  static_cast<uint16_t>(flag_C);
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
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(X) +
                  static_cast<uint16_t>(flag_C);
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
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(X) +
                  static_cast<uint16_t>(flag_C);
  AddCycles(1);
  WriteMemory(addr, A);
}

void Cpu::StaAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(Y) +
                  static_cast<uint16_t>(flag_C);
  AddCycles(1);
  WriteMemory(addr, A);
}

void Cpu::StaIndirectX() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = (hi << 8) | lo;
  WriteMemory(addr, A);
}

void Cpu::StaIndirectY() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = ((hi << 8) | lo) + static_cast<uint16_t>(Y) +
                  static_cast<uint16_t>(flag_C);
  AddCycles(1);
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
  UpdateNZ(SP);
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
void Cpu::PhaImplied() { Push(A); }

/******************************************************************
  PHP
******************************************************************/
void Cpu::PhpImplied() {
  Push(0b00000000 | (static_cast<uint8_t>(flag_N) << 7) |
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
  A = Pop();
  AddCycles(1);
  UpdateNZ(A);
}

/******************************************************************
  PLP
******************************************************************/
void Cpu::PlpImplied() {
  uint8_t status = Pop();
  flag_N = static_cast<bool>((status >> 7) & 0x1);
  flag_V = static_cast<bool>((status >> 6) & 0x1);
  flag_D = static_cast<bool>((status >> 3) & 0x1);
  flag_I = static_cast<bool>((status >> 2) & 0x1);
  flag_Z = static_cast<bool>((status >> 1) & 0x1);
  flag_C = static_cast<bool>((status >> 0) & 0x1);
  AddCycles(1);
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
  uint16_t addr = ((hi << 8) | lo) + X + static_cast<uint16_t>(flag_C);
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
  uint16_t addr = ((hi << 8) | lo) + X + static_cast<uint16_t>(flag_C);
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
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
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
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = (hi << 8) | lo;
  Adc(ReadMemory(addr));
}

void Cpu::AdcIndirectY() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = ((hi << 8) | lo);
  if (addr / 256 <
      (addr + static_cast<uint16_t>(Y) + static_cast<uint16_t>(flag_C)) / 256) {
    AddCycles(1);
  }
  Adc(ReadMemory(addr + static_cast<uint16_t>(Y) +
                 static_cast<uint16_t>(flag_C)));
}

void Cpu::Adc(uint8_t value) {
  uint8_t old = A;                     // record old A
  A += value;                          // perform add without carry
  flag_C = A < old;                    // test carry flag
  A += static_cast<uint16_t>(flag_C);  // add carry
  flag_C = flag_C || (A < old);        // test carry flag
  UpdateNZV(old, A);                   // test NZV flags
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
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
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
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = (hi << 8) | lo;
  Sbc(ReadMemory(addr));
}

void Cpu::SbcIndirectY() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = ((hi << 8) | lo);
  if (addr / 256 <
      (addr + static_cast<uint16_t>(Y) + static_cast<uint16_t>(flag_C)) / 256) {
    AddCycles(1);
  }
  Sbc(ReadMemory(addr + static_cast<uint16_t>(Y) +
                 static_cast<uint16_t>(flag_C)));
}

inline void Cpu::Sbc(uint8_t value) { Cpu::Adc(~value); }

/******************************************************************
  AND
******************************************************************/
void Cpu::AndImmediate() { And(Fetch()); }

void Cpu::AndZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  And(ReadMemory(addr));
}

void Cpu::AndZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
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
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = (hi << 8) | lo;
  And(ReadMemory(addr));
}

void Cpu::AndIndirectY() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = ((hi << 8) | lo);
  if (addr / 256 <
      (addr + static_cast<uint16_t>(Y) + static_cast<uint16_t>(flag_C)) / 256) {
    AddCycles(1);
  }
  And(ReadMemory(addr + static_cast<uint16_t>(Y) +
                 static_cast<uint16_t>(flag_C)));
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
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
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
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = (hi << 8) | lo;
  Eor(ReadMemory(addr));
}

void Cpu::EorIndirectY() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = ((hi << 8) | lo);
  if (addr / 256 <
      (addr + static_cast<uint16_t>(Y) + static_cast<uint16_t>(flag_C)) / 256) {
    AddCycles(1);
  }
  Eor(ReadMemory(addr + static_cast<uint16_t>(Y) +
                 static_cast<uint16_t>(flag_C)));
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
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
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
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = (hi << 8) | lo;
  Ora(ReadMemory(addr));
}

void Cpu::OraIndirectY() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = ((hi << 8) | lo);
  if (addr / 256 <
      (addr + static_cast<uint16_t>(Y) + static_cast<uint16_t>(flag_C)) / 256) {
    AddCycles(1);
  }
  Ora(ReadMemory(addr + static_cast<uint16_t>(Y) +
                 static_cast<uint16_t>(flag_C)));
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
void Cpu::Cmp(uint8_t reg, uint8_t value) {
  uint8_t old = reg;
  reg += value;
  flag_C = reg < old;
  reg += static_cast<uint16_t>(flag_C);
  flag_C = flag_C || (reg < old);
  UpdateNZ(reg);
}

/******************************************************************
  CMP
******************************************************************/
void Cpu::CmpImmediate() { Cmp(A, ~Fetch()); }

void Cpu::CmpZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Cmp(A, ~ReadMemory(addr));
}

void Cpu::CmpZeroPageX() {
  uint16_t addr = static_cast<uint16_t>(Fetch()) + static_cast<uint16_t>(X);
  AddCycles(1);
  Cmp(A, ~ReadMemory(addr));
}

void Cpu::CmpAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Cmp(A, ~ReadMemory(addr));
}

void Cpu::CmpAbsoluteX() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(X))) / 256) {
    AddCycles(1);
  }
  Cmp(A, ~ReadMemory(addr + static_cast<uint16_t>(X)));
}

void Cpu::CmpAbsoluteY() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  if ((addr / 256) < ((addr + static_cast<uint16_t>(Y))) / 256) {
    AddCycles(1);
  }
  Cmp(A, ~ReadMemory(addr + static_cast<uint16_t>(Y)));
}

void Cpu::CmpIndirectX() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch() + X);
  AddCycles(1);
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = (hi << 8) | lo;
  Cmp(A, ~ReadMemory(addr));
}

void Cpu::CmpIndirectY() {
  uint16_t indirect_addr = static_cast<uint16_t>(Fetch());
  uint16_t lo = static_cast<uint16_t>(ReadMemory(indirect_addr));
  uint16_t hi = static_cast<uint16_t>(ReadMemory(indirect_addr + 1));
  uint16_t addr = ((hi << 8) | lo);
  if (addr / 256 <
      (addr + static_cast<uint16_t>(Y) + static_cast<uint16_t>(flag_C)) / 256) {
    AddCycles(1);
  }
  Cmp(A, ~ReadMemory(addr + static_cast<uint16_t>(Y) +
                     static_cast<uint16_t>(flag_C)));
}

/******************************************************************
  CPX
******************************************************************/
void Cpu::CpxImmediate() { Cmp(X, ~Fetch()); }

void Cpu::CpxZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Cmp(X, ~ReadMemory(addr));
}

void Cpu::CpxAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Cmp(X, ~ReadMemory(addr));
}

/******************************************************************
  CPY
******************************************************************/
void Cpu::CpyImmediate() { Cmp(Y, ~Fetch()); }

void Cpu::CpyZeroPage() {
  uint16_t addr = static_cast<uint16_t>(Fetch());
  Cmp(Y, ~ReadMemory(addr));
}

void Cpu::CpyAbsolute() {
  uint16_t lo = static_cast<uint16_t>(Fetch());
  uint16_t hi = static_cast<uint16_t>(Fetch());
  uint16_t addr = (hi << 8) | lo;
  Cmp(Y, ~ReadMemory(addr));
}

/******************************************************************
  Flag Instructions
******************************************************************/
void Cpu::Clc() { UpdateFlag(flag_C, false); }

void Cpu::Cld() { UpdateFlag(flag_D, false); }

void Cpu::Cli() { UpdateFlag(flag_I, false); }

void Cpu::Clv() { UpdateFlag(flag_V, false); }

void Cpu::Sec() { UpdateFlag(flag_C, true); }

void Cpu::Sed() { UpdateFlag(flag_D, true); }

void Cpu::Sei() { UpdateFlag(flag_I, true); }

inline void Cpu::UpdateFlag(bool& flag, bool value) {
  flag = value;
  AddCycles(1);
}

/*****************************************************************
 *  Utility
 *****************************************************************/

void Cpu::Push(uint8_t value) {
  WriteMemory(static_cast<uint16_t>(SP++), value);
  AddCycles(1);
}

uint8_t Cpu::Pop() {
  uint8_t value = ReadMemory(static_cast<uint16_t>(SP--));
  AddCycles(1);
  return value;
}

inline void Cpu::UpdateNZV(uint8_t old, uint8_t byte) {
  UpdateNZ(byte);
  bool old7 = static_cast<bool>(old >> 7);
  bool new7 = static_cast<bool>(byte >> 7);
  flag_V = (!old7 && !new7 && flag_C) || (old7 && new7 && !flag_C);
}

inline void Cpu::UpdateNZ(uint8_t byte) {
  flag_N = byte >> 7;
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

inline uint8_t Cpu::Fetch() {
  AddCycles(1);
  return mmu.Read(PC++);
}

void Cpu::AddCycles(uint64_t n) { cycles += n; }

}  // namespace cpu