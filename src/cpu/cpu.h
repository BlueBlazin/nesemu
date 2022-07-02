#ifndef SRC_CPU_CPU_H_
#define SRC_CPU_CPU_H_

#include <cstdint>
#include <string>
//
#include <fstream>
#include <iostream>

#include "src/controllers/controllers.h"
#include "src/cpu/event.h"
#include "src/memory/memory.h"

namespace cpu {

enum class InterruptType {
  Nmi,
  Irq,
};

enum class DmaState {
  PreDma,
  OddCycleWait,
  Running,
};

class Cpu {
 public:
  Cpu(const std::string& path);

  void Startup();
  Event RunTillEvent(uint64_t max_cycles);
  void Run();
  void Tick();

  void PressKey(controllers::Key key);
  void ReleaseKey(controllers::Key key);

  uint8_t* GetScreen();
  uint8_t* GetPatTable1();
  uint8_t* GetPatTable2();
  uint8_t* GetNametable(uint16_t addr);

  std::ofstream myfile;

 private:
  void RunDma();

  void DecodeExecute(uint8_t opcode);

  uint16_t IndirectX();
  uint16_t IndirectY();
  uint16_t ZeroPageX();
  uint16_t ZeroPageY();
  uint16_t ZeroPage();
  uint16_t Absolute();
  uint16_t AbsoluteX();
  uint16_t AbsoluteY();

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
  void Ldx(uint16_t addr);

  /* LDY */
  void LdyImmediate();
  void LdyZeroPage();
  void LdyZeroPageX();
  void LdyAbsolute();
  void LdyAbsoluteX();
  void Ldy(uint16_t addr);

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
  void Dec(uint16_t addr, uint8_t value);
  /* DEX */
  void DexImplied();
  /* DEY */
  void DeyImplied();

  /* INC */
  void IncZeroPage();
  void IncZeroPageX();
  void IncAbsolute();
  void IncAbsoluteX();
  void Inc(uint16_t addr, uint8_t value);
  /* INX */
  void InxImplied();
  /* INY */
  void InyImplied();

  /* ADC */
  bool Overflow(int8_t reg, int8_t value, int8_t carry);
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
  void Asl(uint16_t addr, uint8_t value);

  /* LSR */
  void LsrAccumulator();
  void LsrZeroPage();
  void LsrZeroPageX();
  void LsrAbsolute();
  void LsrAbsoluteX();
  void Lsr(uint16_t addr, uint8_t value);

  /* ROL */
  void RolAccumulator();
  void RolZeroPage();
  void RolZeroPageX();
  void RolAbsolute();
  void RolAbsoluteX();
  void Rol(uint16_t addr, uint8_t value);

  /* ROR */
  void RorAccumulator();
  void RorZeroPage();
  void RorZeroPageX();
  void RorAbsolute();
  void RorAbsoluteX();
  void Ror(uint16_t addr, uint8_t value);

  /* Flag Instructions */
  void ClcImplied();
  void CldImplied();
  void CliImplied();
  void ClvImplied();
  void SecImplied();
  void SedImplied();
  void SeiImplied();
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
  void Branch(bool condition);

  /* Jump and Subroutine Instructions */
  void JmpAbsolute();
  void JmpIndirect();
  void JsrAbsolute();
  void RtsImplied();

  /* Interrupt Instructions */
  void BrkImplied();
  void RtiImplied();
  void Interrupt(InterruptType type);

  /* BIT */
  void BitZeroPage();
  void BitAbsolute();
  void Bit(uint16_t addr);

  /* Utility methods */
  bool NmiPending();
  bool IrqPending();
  void Push(uint8_t value);
  uint8_t Pull(uint8_t SP);
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
  uint64_t event_cycles = 0;
  DmaState dma_state = DmaState::PreDma;

  /* Registers */
  uint8_t A = 0x0;
  uint8_t X = 0x0;
  uint8_t Y = 0x0;
  uint8_t SP = 0xFD;
  uint16_t PC = 0x0;

  /* Flags (processor status) */
  bool flag_N = false;
  bool flag_V = false;
  bool flag_D = false;
  bool flag_I = true;
  bool flag_Z = false;
  bool flag_C = false;
};

}  // namespace cpu

#endif  // SRC_CPU_CPU_H_
