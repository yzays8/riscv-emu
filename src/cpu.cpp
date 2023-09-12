#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>
#include <bitset>

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
  // std::cout << "instr: 0x" << std::hex << static_cast<int>(instr) <<", opcode: 0x" << static_cast<int>(opcode) << ", 0b" << std::bitset<7>(opcode) << " |||" << std::endl;

  switch (opcode) {
  case 0b0000011:
    switch (decoded.i_type.funct3) {
    case 0b000: // lb
      regs_[decoded.i_type.rd] = static_cast<uint64_t>(static_cast<int8_t>(bus_->Load(static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12), 8)));
      break;
    case 0b001: // lh
      regs_[decoded.i_type.rd] = static_cast<uint64_t>(static_cast<int16_t>(bus_->Load(static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12), 16)));
      break;
    case 0b010: // lw
      regs_[decoded.i_type.rd] = static_cast<uint64_t>(static_cast<int32_t>(bus_->Load(static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12), 32)));
      break;
    case 0b011: // ld, RV64I
      regs_[decoded.i_type.rd] = bus_->Load(static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12), 64);
      break;
    case 0b100: // lbu
      regs_[decoded.i_type.rd] = bus_->Load(static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12), 8);
      break;
    case 0b101: // lhu
      regs_[decoded.i_type.rd] = bus_->Load(static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12), 16);
      break;
    case 0b110: // lwu, RV64I
      regs_[decoded.i_type.rd] = bus_->Load(static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12), 32);
      break;
    default:
      std::cerr << "Unknown funct3: 0b" << std::bitset<3>(decoded.i_type.funct3) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
      std::exit(EXIT_FAILURE);
    }
    break;
  case 0b0010011: {
    uint64_t shamt = decoded.i_type.imm_11_0 & 0x3F;  // RV64I
    switch (decoded.i_type.funct3) {
    case 0b000: // addi
      regs_[decoded.i_type.rd] = static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12);
      break;
    case 0b001: // slli, RV64I
      regs_[decoded.i_type.rd] = static_cast<uint64_t>(regs_[decoded.i_type.rs1] << shamt);
      break;
    case 0b010: // slti
      regs_[decoded.i_type.rd] = static_cast<uint64_t>((static_cast<int64_t>(regs_[decoded.i_type.rs1]) < SignExtend<int64_t>(decoded.i_type.imm_11_0, 12)) ? 1u : 0u);
      break;
    case 0b011: // sltiu
      regs_[decoded.i_type.rd] = static_cast<uint64_t>((static_cast<int64_t>(regs_[decoded.i_type.rs1]) < SignExtend<int64_t>(decoded.i_type.imm_11_0, 12)) ? 1u : 0u);
      break;
    case 0b100: // xori
      regs_[decoded.i_type.rd] = regs_[decoded.i_type.rs1] ^ SignExtend<uint64_t>(decoded.i_type.imm_11_0, 12);
      break;
    case 0b101: {
      uint8_t imm_11_6 = decoded.i_type.imm_11_0 >> 6;
      switch (imm_11_6) {
      case 0b000000: // srli, RV64I
        regs_[decoded.i_type.rd] = static_cast<uint64_t>(regs_[decoded.i_type.rs1] >> shamt);
        break;
      case 0b010000: // srai, RV64I
        regs_[decoded.i_type.rd] = static_cast<uint64_t>(static_cast<int64_t>(regs_[decoded.i_type.rs1]) >> shamt);
        break;
      default:
        std::cerr << "Unknown imm_11_6: 0b" << std::bitset<6>(imm_11_6) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
        std::exit(EXIT_FAILURE);
      }
      break;
    }
    case 0b110: // ori
      regs_[decoded.i_type.rd] = regs_[decoded.i_type.rs1] | SignExtend<uint64_t>(decoded.i_type.imm_11_0, 12);
      break;
    case 0b111: // andi
      regs_[decoded.i_type.rd] = regs_[decoded.i_type.rs1] & SignExtend<uint64_t>(decoded.i_type.imm_11_0, 12);
      break;
    default:
      assert(false);
    }
    break;
  }
  case 0b0010111: // auipc
    // AUIPC forms a 32-bit offset from the 20-bit U-immediate, filling in the lowest 12 bits
    // with zeros, adds this offset to the pc, then places the result in register rd.
    regs_[decoded.u_type.rd] = static_cast<int64_t>(pc_) + static_cast<int64_t>(decoded.u_type.imm_31_12 << 12);
    break;
  case 0b0011011: {
    uint64_t shamt = decoded.i_type.imm_11_0 & 0x1F;
    switch (decoded.i_type.funct3) {
    case 0b000: // addiw, RV64I
      regs_[decoded.i_type.rd] = static_cast<int32_t>(static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12));
      break;
    case 0b001: // slliw, RV64I
      regs_[decoded.i_type.rd] = static_cast<int32_t>(regs_[decoded.i_type.rs1] << shamt);
      break;
    case 0b101: {
      uint8_t imm_11_5 = decoded.i_type.imm_11_0 >> 5;
      switch (imm_11_5) {
      case 0b0000000: // srliw, RV64I
        regs_[decoded.i_type.rd] = static_cast<int32_t>(regs_[decoded.i_type.rs1] >> shamt);
        break;
      case 0b0100000: // sraiw, RV64I
        regs_[decoded.i_type.rd] = static_cast<int32_t>(static_cast<int64_t>(regs_[decoded.i_type.rs1]) >> shamt);
        break;
      default:
        std::cerr << "Unknown imm_11_5: 0b" << std::bitset<7>(imm_11_5) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
        std::exit(EXIT_FAILURE);
      }
      break;
    }
    default:
      std::cerr << "Unknown funct3: 0b" << std::bitset<3>(decoded.i_type.funct3) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
      std::exit(EXIT_FAILURE);
    }
    break;
  }
  case 0b0100011: {
    int64_t imm = SignExtend<int64_t>((decoded.s_type.imm_11_5 << 5) | decoded.s_type.imm_4_0, 12);
    switch (decoded.s_type.funct3) {
    case 0b000: // sb
      bus_->Store(static_cast<int64_t>(regs_[decoded.s_type.rs1]) + imm, 8, static_cast<uint8_t>(regs_[decoded.s_type.rs2]));
      break;
    case 0b001: // sh
      bus_->Store(static_cast<int64_t>(regs_[decoded.s_type.rs1]) + imm, 16, static_cast<uint16_t>(regs_[decoded.s_type.rs2]));
      break;
    case 0b010: // sw
      bus_->Store(static_cast<int64_t>(regs_[decoded.s_type.rs1]) + imm, 32, static_cast<uint32_t>(regs_[decoded.s_type.rs2]));
      break;
    case 0b011: // sd, RV64I
      bus_->Store(static_cast<int64_t>(regs_[decoded.s_type.rs1]) + imm, 64, regs_[decoded.s_type.rs2]);
      break;
    default:
      std::cerr << "Unknown funct3: 0b" << std::bitset<3>(decoded.s_type.funct3) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
      std::exit(EXIT_FAILURE);
    }
    break;
  }
  case 0b0110011:
    switch (decoded.r_type.funct3) {
    case 0b000:
      switch (decoded.r_type.funct7) {
      case 0b0000000: // add
        regs_[decoded.r_type.rd] = regs_[decoded.r_type.rs1] + regs_[decoded.r_type.rs2];
        break;
      case 0b0100000: // sub
        regs_[decoded.r_type.rd] = regs_[decoded.r_type.rs1] - regs_[decoded.r_type.rs2];
        break;
      default:
        std::cerr << "Unknown funct7: 0b" << std::bitset<7>(decoded.r_type.funct7) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
        std::exit(EXIT_FAILURE);
      }
      break;
    case 0b001: // sll
      regs_[decoded.r_type.rd] = static_cast<uint64_t>(regs_[decoded.r_type.rs1] << regs_[decoded.r_type.rs2]);
      break;
    case 0b010: // slt
      regs_[decoded.r_type.rd] = static_cast<uint64_t>((static_cast<int64_t>(regs_[decoded.r_type.rs1]) < static_cast<int64_t>(regs_[decoded.r_type.rs2])) ? 1u : 0u);
      break;
    case 0b011: // sltu
      regs_[decoded.r_type.rd] = static_cast<uint64_t>((regs_[decoded.r_type.rs1] < regs_[decoded.r_type.rs2]) ? 1u : 0u);
      break;
    case 0b100: // xor
      regs_[decoded.r_type.rd] = regs_[decoded.r_type.rs1] ^ regs_[decoded.r_type.rs2];
      break;
    case 0b101:
      switch (decoded.r_type.funct7) {
      case 0b0000000: // srl
        regs_[decoded.r_type.rd] = static_cast<uint64_t>(regs_[decoded.r_type.rs1] >> regs_[decoded.r_type.rs2]);
        break;
      case 0b0100000: // sra
        regs_[decoded.r_type.rd] = static_cast<uint64_t>(static_cast<int64_t>(regs_[decoded.r_type.rs1]) >> regs_[decoded.r_type.rs2]);
        break;
      default:
        std::cerr << "Unknown funct7: 0b" << std::bitset<7>(decoded.r_type.funct7) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
        std::exit(EXIT_FAILURE);
      }
      break;
    case 0b110: // or
      regs_[decoded.r_type.rd] = regs_[decoded.r_type.rs1] | regs_[decoded.r_type.rs2];
      break;
    case 0b111: // and
      regs_[decoded.r_type.rd] = regs_[decoded.r_type.rs1] & regs_[decoded.r_type.rs2];
      break;
    default:
      assert(false);
    }
    break;
  case 0b0110111: // lui
    regs_[decoded.u_type.rd] = SignExtend<int64_t>(decoded.u_type.imm_31_12 << 12, 32);
    break;
  case 0b0111011:
    switch (decoded.r_type.funct3) {
    case 0b000:
      switch (decoded.r_type.funct7) {
      case 0b0000000: // addw, RV64I
        regs_[decoded.r_type.rd] = static_cast<int32_t>(static_cast<int64_t>(regs_[decoded.r_type.rs1]) + static_cast<int64_t>(regs_[decoded.r_type.rs2]));
        break;
      case 0b0100000: // subw, RV64I
        regs_[decoded.r_type.rd] = static_cast<int32_t>(static_cast<int64_t>(regs_[decoded.r_type.rs1]) - static_cast<int64_t>(regs_[decoded.r_type.rs2]));
        break;
      default:
        std::cerr << "Unknown funct7: 0b" << std::bitset<7>(decoded.r_type.funct7) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
        std::exit(EXIT_FAILURE);
      }
      break;
    case 0b001: // sllw, RV64I
      regs_[decoded.r_type.rd] = static_cast<int32_t>(regs_[decoded.r_type.rs1] << (regs_[decoded.r_type.rs2] & 0x1F));
      break;
    case 0b101:
      switch (decoded.r_type.funct7) {
      case 0b0000000: // srlw, RV64I
        regs_[decoded.r_type.rd] = static_cast<int32_t>(static_cast<uint32_t>(regs_[decoded.r_type.rs1]) >> (regs_[decoded.r_type.rs2] & 0x1F));
        break;
      case 0b0100000: // sraw, RV64I
        regs_[decoded.r_type.rd] = static_cast<int32_t>(static_cast<int32_t>(regs_[decoded.r_type.rs1]) >> (regs_[decoded.r_type.rs2] & 0x1F));
        break;
      default:
        std::cerr << "Unknown funct7: 0b" << std::bitset<7>(decoded.r_type.funct7) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
        std::exit(EXIT_FAILURE);
      }
      break;
    default:
      std::cerr << "Unknown funct3: 0b" << std::bitset<3>(decoded.r_type.funct3) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
      std::exit(EXIT_FAILURE);
    }
    break;
  case 0b1100011: {
    int64_t imm = SignExtend<int64_t>((decoded.b_type.imm_12 << 12) | (decoded.b_type.imm_11 << 11) | (decoded.b_type.imm_10_5 << 5) | (decoded.b_type.imm_4_1 << 1), 13);
    switch (decoded.b_type.funct3) {
    case 0b000: // beq
      if (regs_[decoded.b_type.rs1] == regs_[decoded.b_type.rs2]) {
        return static_cast<uint64_t>(static_cast<int64_t>(pc_) + imm);
      }
      break;
    case 0b001: // bne
      if (regs_[decoded.b_type.rs1] != regs_[decoded.b_type.rs2]) {
        return static_cast<uint64_t>(static_cast<int64_t>(pc_) + imm);
      }
      break;
    case 0b100: // blt
      if (static_cast<int64_t>(regs_[decoded.b_type.rs1]) < static_cast<int64_t>(regs_[decoded.b_type.rs2])) {
        return static_cast<uint64_t>(static_cast<int64_t>(pc_) + imm);
      }
      break;
    case 0b101: // bge
      if (static_cast<int64_t>(regs_[decoded.b_type.rs1]) >= static_cast<int64_t>(regs_[decoded.b_type.rs2])) {
        return static_cast<uint64_t>(static_cast<int64_t>(pc_) + imm);
      }
      break;
    case 0b110: // bltu
      if (regs_[decoded.b_type.rs1] < regs_[decoded.b_type.rs2]) {
        return static_cast<uint64_t>(static_cast<int64_t>(pc_) + imm);
      }
      break;
    case 0b111: // bgeu
      if (regs_[decoded.b_type.rs1] >= regs_[decoded.b_type.rs2]) {
        return static_cast<uint64_t>(static_cast<int64_t>(pc_) + imm);
      }
      break;
    default:
      std::cerr << "Unknown funct3: 0b" << std::bitset<3>(decoded.b_type.funct3) << " in opcode: 0b" << std::bitset<7>(opcode) << std::endl;
      std::exit(EXIT_FAILURE);
    }
    break;
  }
  case 0b1100111: { // jalr
    uint64_t ret = static_cast<uint64_t>((static_cast<int64_t>(regs_[decoded.i_type.rs1]) + SignExtend<int64_t>(decoded.i_type.imm_11_0, 12)) & ~1);
    // If rd is x0, the return address is not saved.
    if (decoded.i_type.rd != 0b00000) {
      regs_[decoded.i_type.rd] = pc_ + 4;
    }
    return ret;
  }
  case 0b1101111: { // jal
    // If rd is x0, the return address is not saved.
    if (decoded.j_type.rd != 0b00000) {
      regs_[decoded.j_type.rd] = pc_ + 4;
    }
    int64_t imm = SignExtend<int64_t>((decoded.j_type.imm_20 << 20) | (decoded.j_type.imm_19_12 << 12) | (decoded.j_type.imm_11 << 11) | (decoded.j_type.imm_10_1 << 1), 21);
    return static_cast<uint64_t>(static_cast<int64_t>(pc_) + imm);
  }
  default:
    std::cerr << "Unknown opcode: 0b" << std::bitset<7>(opcode) << std::endl;
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
  printf("PC: 0x%lx", pc_);
  std::cout << "\n---------------------" << std::endl;
}
