#ifndef SRC_CPU_CPU_
#define SRC_CPU_CPU_

#include <cstdint>

#include "src/memory/memory.h"

namespace cpu {

constexpr uint8_t FLAG_N = 0x1 << 7;  // Negative
constexpr uint8_t FLAG_V = 0x1 << 6;  // oVerflow
                                      // unused
constexpr uint8_t FLAG_B = 0x1 << 4;  // Break
constexpr uint8_t FLAG_D = 0x1 << 3;  // Decimal
constexpr uint8_t FLAG_I = 0x1 << 2;  // Interrupt
constexpr uint8_t FLAG_Z = 0x1 << 1;  // Zero
constexpr uint8_t FLAG_C = 0x1;       // Carry

constexpr uint8_t invert(uint8_t flag) { return 0xFF ^ flag; }

class Cpu {
 public:
  Cpu();

  void Run();

 private:
  /* LDA */
  void LdaImmediate();
  void LdaZeroPage();
  void LdaZeroPageX();
  void LdaAbsolute();
  void LdaAbsoluteX();
  void LdaAbsoluteY();
  void LdaIndirectX();
  void LdaIndirectY();

  /* LDX */
  void LdxImmediate();
  void LdxZeroPage();
  void LdxZeroPageY();
  void LdxAbsolute();
  void LdxAbsoluteY();

  /* LDY */
  void LdyImmediate();
  void LdyZeroPage();
  void LdyZeroPageX();
  void LdyAbsolute();
  void LdyAbsoluteX();

  /* STA */
  void StaZeroPage();
  void StaZeroPageX();
  void StaAbsolute();
  void StaAbsoluteX();
  void StaAbsoluteY();
  void StaIndirectX();
  void StaIndirectY();

  /* STX */
  void StxZeroPage();
  void StxZeroPageY();
  void StxAbsolute();

  /* STY */
  void StyZeroPage();
  void StyZeroPageX();
  void StyAbsolute();

  /* TAX */
  void TaxImplied();
  /* TAY */
  void TayImplied();
  /* TSX */
  void TsxImplied();
  /* TXA */
  void TxaImplied();
  /* TXS */
  void TxsImplied();
  /* TYA */
  void TyaImplied();

  /* PHA */
  void PhaImplied();
  /* PHP */
  void PhpImplied();
  /* PLA */
  void PlaImplied();
  /* PLP */
  void PlpImplied();

  /* DEC */
  void DecZeroPage();
  void DecZeroPageX();
  void DecAbsolute();
  void DecAbsoluteX();
  /* DEX */
  void DexImplied();
  /* DEY */
  void DeyImplied();

  /* INC */
  void IncZeroPage();
  void IncZeroPageX();
  void IncAbsolute();
  void IncAbsoluteX();
  /* INX */
  void InxImplied();
  /* INY */
  void InyImplied();

  /* ADC */
  void AdcImmediate();
  void AdcZeroPage();
  void AdcZeroPageX();
  void AdcAbsolute();
  void AdcAbsoluteX();
  void AdcAbsoluteY();
  void AdcIndirectX();
  void AdcIndirectY();
  void Adc(uint8_t value);

  /* SBC */
  void SbcImmediate();
  void SbcZeroPage();
  void SbcZeroPageX();
  void SbcAbsolute();
  void SbcAbsoluteX();
  void SbcAbsoluteY();
  void SbcIndirectX();
  void SbcIndirectY();
  void Sbc(uint8_t value);

  void Push(uint8_t value);
  uint8_t Pop();
  void UpdateNZV(uint8_t old, uint8_t byte);
  void UpdateNZ(uint8_t byte);
  uint8_t Fetch();

  uint8_t ReadMemory(uint16_t addr);
  void WriteMemory(uint16_t addr, uint8_t value);

  void AddCycles(uint64_t n);

  /* Memory */
  memory::Memory mmu;

  /* Internal */
  uint64_t cycles = 0;

  /* Registers */
  uint8_t A = 0x0;
  uint8_t X = 0x0;
  uint8_t Y = 0x0;
  uint8_t SP = 0xFF;
  uint16_t PC = 0x0;

  /* Flags (processor status) */
  bool flag_N = false;
  bool flag_Z = false;
  bool flag_C = false;
  bool flag_I = false;
  bool flag_D = false;
  bool flag_V = false;
  bool flag_B = false;
};

}  // namespace cpu

#endif
