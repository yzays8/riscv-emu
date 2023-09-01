#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <vector>

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

constexpr uint64_t kDramSize = static_cast<uint64_t>(1024 * 1024 * 128); // 128 MiB

class CPU {
 public:
  CPU();
  void LoadProgram(const std::string& path);
  void Run();

 private:
  uint32_t Fetch();
  void Execute(uint32_t instr);
  void PrintRegs() const;

  std::array<uint64_t, 32> regs_;
  uint64_t pc_;
  std::vector<uint8_t> dram_;
};
