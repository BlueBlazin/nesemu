#ifndef SRC_CPU_CPU_
#define SRC_CPU_CPU_

#include <cstdint>

#include "src/memory/memory.h"

namespace cpu {

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

  /* AND */
  void AndImmediate();
  void AndZeroPage();
  void AndZeroPageX();
  void AndAbsolute();
  void AndAbsoluteX();
  void AndAbsoluteY();
  void AndIndirectX();
  void AndIndirectY();
  void And(uint8_t value);

  /* EOR */
  void EorImmediate();
  void EorZeroPage();
  void EorZeroPageX();
  void EorAbsolute();
  void EorAbsoluteX();
  void EorAbsoluteY();
  void EorIndirectX();
  void EorIndirectY();
  void Eor(uint8_t value);

  /* ORA */
  void OraImmediate();
  void OraZeroPage();
  void OraZeroPageX();
  void OraAbsolute();
  void OraAbsoluteX();
  void OraAbsoluteY();
  void OraIndirectX();
  void OraIndirectY();
  void Ora(uint8_t value);

  /* ASL */
  void AslAccumulator();
  void AslZeroPage();
  void AslZeroPageX();
  void AslAbsolute();
  void AslAbsoluteX();
  void Asl(uint16_t addr);

  /* LSR */
  void LsrAccumulator();
  void LsrZeroPage();
  void LsrZeroPageX();
  void LsrAbsolute();
  void LsrAbsoluteX();
  void Lsr(uint16_t addr);

  /* ROL */
  void RolAccumulator();
  void RolZeroPage();
  void RolZeroPageX();
  void RolAbsolute();
  void RolAbsoluteX();
  void Rol(uint16_t addr);

  /* ROR */
  void RorAccumulator();
  void RorZeroPage();
  void RorZeroPageX();
  void RorAbsolute();
  void RorAbsoluteX();
  void Ror(uint16_t addr);

  /* Flag Instructions */
  void Clc();
  void Cld();
  void Cli();
  void Clv();
  void Sec();
  void Sed();
  void Sei();
  void UpdateFlag(bool& flag, bool value);

  /* CMP */
  void CmpImmediate();
  void CmpZeroPage();
  void CmpZeroPageX();
  void CmpAbsolute();
  void CmpAbsoluteX();
  void CmpAbsoluteY();
  void CmpIndirectX();
  void CmpIndirectY();

  /* CPX */
  void CpxImmediate();
  void CpxZeroPage();
  void CpxAbsolute();

  /* CPY */
  void CpyImmediate();
  void CpyZeroPage();
  void CpyAbsolute();

  void Cmp(uint8_t reg, uint8_t value);

  /* Conditional Branch Instructions */
  void BccRelative();
  void BcsRelative();
  void BeqRelative();
  void BmiRelative();
  void BneRelative();
  void BplRelative();
  void BvcRelative();
  void BvsRelative();

  /* Jump and Subroutine Instructions */
  void JmpAbsolute();
  void JmpIndirect();
  void JsrAbsolute();
  void RtsImplied();

  /* Interrupt Instructions */
  void Brk();
  void Rti();

  /* Utility methods */
  void Push2(uint16_t value);
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
