#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "dram.hpp"

DRAM::DRAM(const std::string& prog_path) : dram_{} {
  LoadProgram(prog_path);
}

void DRAM::LoadProgram(const std::string& path) {
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
  for (int i = 0; ifs.get(data); ++i) {
    dram_[i] = static_cast<uint8_t>(data);
  }
}

uint64_t DRAM::Load(uint64_t addr, int size) const {
  if (size != 8 && size != 16 && size != 32 && size != 64) {
    std::cerr << "Invalid load size: " << size << std::endl;
    std::exit(EXIT_FAILURE);
  }
  if ((addr < kDramBaseAddr) || (kDramBaseAddr + kDramSize <= addr)) {
    std::cerr << "Invalid load address: " << addr << std::endl;
    std::exit(EXIT_FAILURE);
  }

  uint64_t dram_addr = addr - kDramBaseAddr;
  uint64_t data = 0;
  const int kLoadBytes = size / 8;
  for (int i = 0; i < kLoadBytes; ++i) {
    data |= static_cast<uint64_t>(dram_[dram_addr + i] << (8 * i));
  }

  return data;
}

void DRAM::Store(uint64_t addr, int size, uint64_t data) {
  if (size != 8 && size != 16 && size != 32 && size != 64) {
    std::cerr << "Invalid load size: " << size << std::endl;
    std::exit(EXIT_FAILURE);
  }
  if ((addr < kDramBaseAddr) || (kDramBaseAddr + kDramSize <= addr)) {
    std::cerr << "Invalid load address: " << addr << std::endl;
    std::exit(EXIT_FAILURE);
  }

  uint64_t dram_addr = addr - kDramBaseAddr;
  const int kLoadBytes = size / 8;
  for (int i = 0; i < kLoadBytes; ++i) {
    data >>= 8;
    dram_[dram_addr + i] = static_cast<uint8_t>(data & 0xFF);
  }
}
