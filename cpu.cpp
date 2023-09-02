#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>

#include "cpu.hpp"

CPU::CPU(const std::string& prog_path)
    : regs_{}, pc_{kDramBaseAddr}, bus_{std::make_unique<Bus>(prog_path)} {
  regs_[2] = kDramBaseAddr + kDramSize - 1; // sp
}

uint32_t CPU::Fetch() {
  return static_cast<uint32_t>(bus_->Load(pc_, 32));
}

uint64_t CPU::Execute(uint32_t instr) {
  DecodedType decoded = static_cast<DecodedType>(instr);
  uint8_t opcode = decoded.i_type.opcode; // i_type is just a random choice

  switch (opcode) {
  case 0b0010011: // addi
    regs_[decoded.i_type.rd] = regs_[decoded.i_type.rs1] + decoded.i_type.imm_11_0;
    break;
  case 0b0110011: // add
    regs_[decoded.r_type.rd] = regs_[decoded.r_type.rs1] + regs_[decoded.r_type.rs2];
    break;
  default:
    std::cerr << "Unknown opcode: " << opcode << std::endl;
    std::exit(EXIT_FAILURE);
  }

  return pc_ + 4;
}

void CPU::SetPC(uint64_t pc) {
  pc_ = pc;
}

void CPU::PrintRegs() const {
  int i = 0;
  for (auto reg : regs_) {
    printf("x%d: 0x%lx, ", i++, reg);
  }
  std::cout << "\n---------------------" << std::endl;
}
