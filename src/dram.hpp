#pragma once

#include <cstdint>
#include <array>
#include <string>

constexpr uint64_t kDramSize = static_cast<uint64_t>(1024 * 1024 * 128); // 128 MiB
constexpr uint64_t kDramBaseAddr = 0x8000'0000; // DRAM starts at 0x8000'0000 in memory space

class DRAM {
 public:
  DRAM(const std::string& prog_path);
  void LoadProgram(const std::string& path);
  uint64_t Load(uint64_t addr, int size) const;
  void Store(uint64_t addr, int size, uint64_t data);

 private:
  std::array<uint8_t, kDramSize> dram_;
};
