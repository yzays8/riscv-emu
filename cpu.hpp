#pragma once

#include <iostream>
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
    uint32_t imm_11 : 1;
    uint32_t imm_4_1 : 4;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t imm_10_5 : 6;
    uint32_t imm_12 : 1;
  } b_type;
  static_assert(sizeof(b_type) == sizeof(uint32_t));

  struct {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t imm_31_12 : 20;
  } u_type;
  static_assert(sizeof(u_type) == sizeof(uint32_t));

  struct {
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t imm_19_12 : 8;
    uint32_t imm_11 : 1;
    uint32_t imm_10_1 : 10;
    uint32_t imm_20 : 1;
  } j_type;
  static_assert(sizeof(j_type) == sizeof(uint32_t));
};

class CPU {
 public:
  CPU(const std::string& prog_path);
  uint32_t Fetch();
  uint64_t Execute(uint32_t instr);
  void SetPC(uint64_t pc);
  void PrintRegs() const;

 private:
  template <class T> T SignExtend(T val, int bits) const;

  std::array<uint64_t, 32> regs_;
  uint64_t pc_;
  std::unique_ptr<Bus> bus_;
};

template <class T>
T CPU::SignExtend(T val, int bits) const {
  if (static_cast<size_t>(bits) > sizeof(T) * 8) {
    std::cerr << "bits: " << bits << " is larger than sizeof(T): " << sizeof(T) << std::endl;
    std::exit(EXIT_FAILURE);
  }
  T msb_set = 1 << (bits - 1);
  return (val ^ msb_set) - msb_set;
}
