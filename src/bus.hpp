#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include "dram.hpp"

class Bus {
 public:
  Bus(const std::string& prog_path);
  uint64_t Load(uint64_t addr, int size);
  void Store(uint64_t addr, int size, uint64_t data);

 private:
  std::unique_ptr<DRAM> dram_;
};
