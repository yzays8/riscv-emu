#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>

#include "cpu.hpp"

CPU::CPU() : regs_{}, pc_{0}, dram_{} {
  regs_[2] = kDramSize - 1; // sp
}

void CPU::LoadProgram(const std::string& path) {
  namespace fs = std::filesystem;
  if (!fs::is_regular_file(path)) {
    std::cerr << fs::weakly_canonical(fs::absolute(path)) << " is not a regular file" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  std::ifstream ifs{path, std::ios::in | std::ios::binary};
  if (!ifs.is_open()) {
    std::cerr << "Failed to open " << fs::weakly_canonical(fs::absolute(path)) << std::endl;
    std::exit(EXIT_FAILURE);
  }

  char data;
  while (ifs.get(data)) {
    dram_.push_back(static_cast<uint8_t>(data));
  }
}

uint32_t CPU::Fetch() {
  return static_cast<uint32_t>(
    static_cast<uint32_t>(dram_[pc_]) |
    static_cast<uint32_t>(dram_[pc_ + 1]) << 8 |
    static_cast<uint32_t>(dram_[pc_ + 2]) << 16 |
    static_cast<uint32_t>(dram_[pc_ + 3]) << 24
  );
}

void CPU::Execute(uint32_t instr) {
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
}

void CPU::Run() {
  std::cout << "Running..." << std::endl;

  while (pc_ < dram_.size()) {
    PrintRegs();
    Execute(Fetch());
    pc_ += 4;
  }
  PrintRegs();
}

void CPU::PrintRegs() const {
  int i = 0;
  for (auto reg : regs_) {
    printf("x%d: 0x%lx, ", i++, reg);
  }
  std::cout << "\n---------------------" << std::endl;
}
