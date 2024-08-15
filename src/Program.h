#ifndef PROGRAM_H
#define PROGRAM_H

#include "VortexTypes.h"
#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

class Program {
public:
  std::vector<std::uint8_t> Bytecode;
  std::vector<VortexValue> Constants;
  std::vector<std::unique_ptr<Object>>
      Objects; // the VM's memory is here it's kinda weird
public:
  Program() = default;

  auto pushCode(std::uint8_t code, std::size_t line) -> void;
  // Creates a string on the objects list, returns a pointer to it's location on
  // the object store will use to create a VortexValue -> push it as a constant
  // -> access it as an Object *
  auto createString(std::string_view contents) -> Object *;
  auto addConstant(VortexValue constant) -> std::int32_t;

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
