#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <memory>

#include "bus.hpp"

union DecodedType {
  uint32_t raw;

  struct {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t funct7 : 7;
  } r_type;
  static_assert(sizeof(r_type) == sizeof(uint32_t));

  struct {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t imm_11_0 : 12;
  } i_type;
  static_assert(sizeof(i_type) == sizeof(uint32_t));

  struct {
    uint32_t opcode : 7;
    uint32_t imm_4_0 : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t imm_11_5 : 7;
  } s_type;
  static_assert(sizeof(s_type) == sizeof(uint32_t));

  struct {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t imm_31_12 : 20;
  } u_type;
  static_assert(sizeof(u_type) == sizeof(uint32_t));
};

class CPU {
 public:
  CPU(const std::string& prog_path);
  uint32_t Fetch();
  uint64_t Execute(uint32_t instr);
  void SetPC(uint64_t pc);
  void PrintRegs() const;

 private:
  std::array<uint64_t, 32> regs_;
  uint64_t pc_;
  std::unique_ptr<Bus> bus_;
};
