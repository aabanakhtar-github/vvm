#ifndef PROGRAM_H
#define PROGRAM_H

#include "VortexTypes.h"
#include <cstdint>
#include <string_view>
#include <vector>

class Program {
public:
  std::vector<std::uint8_t> Bytecode;
  std::vector<VortexValue> Constants;

public:
  Program() = default;

  auto pushCode(std::uint8_t code, std::size_t line) -> void;
  auto addConstant(VortexValue constant) -> std::size_t;

  auto dissassemble(std::string_view output_filename) -> void;
  auto dissassembleInstruction(std::size_t &i) -> std::string;

  auto getConstant(std::uint8_t index) -> VortexValue const {
    return Constants[index];
  }
  // i is the index of the instruction
private:
  auto dissassembleRegular(std::size_t &i)
      -> std::string; // Dissassemble single byte instructions
  auto dissassembleConstant(std::size_t &i)
      -> std::string; // Dissassemble LD_CONST [2 bytes]
private:
  std::vector<std::size_t> lines_; // TODO: more efficient storage strategy
};

#endif // !PROGRAM_H
