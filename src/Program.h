#ifndef PROGRAM_H
#define PROGRAM_H

#include "VortexTypes.h"
#include <cassert>
#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>
// Represents the program in bytecode and runtime, with all user memory here
class Program {
public:
  std::vector<std::uint8_t> Bytecode;
  std::vector<VortexValue> Constants;
  std::vector<VortexValue> Globals;
  std::vector<std::unique_ptr<Object>>
      Objects; // the VM's memory is here it's kinda weird
public:
  Program() = default;

  auto pushCode(std::uint8_t code, std::size_t line) -> void;
  // Creates a string on the objects list, returns a pointer to it's location on
  // the object store will use to create a VortexValue -> push it as a constant
  // -> access it as an Object *
  auto createString(std::string_view contents) -> Object *;
  // TODO: rename to createConstant
  auto addConstant(VortexValue constant) -> std::int32_t;
  auto createGlobal(std::string_view name, VortexValue value) -> std::size_t;

  auto dissassemble(std::string_view output_filename) -> void;
  auto dissassembleInstruction(std::size_t &i) -> std::string;

  auto getConstant(std::uint32_t index) -> VortexValue const {
    return Constants[index];
  }
  auto getGlobalIndex(std::string_view name) -> std::size_t {
    assert(global_to_index_.contains(std::string{name}) &&
           "Non existent global!");
    return global_to_index_[std::string{name}];
  }

private:
  // i is the index of the instruction, used to update index for dissassembler.
  auto dissassembleRegular(std::size_t &i)
      -> std::string; // Dissassemble single byte instructions
  auto dissassembleConstant(std::size_t &i)
      -> std::string; // Dissassemble PUSHC [4 bytes]
  auto dissassembleLoadGlobal(std::size_t &i);
  auto dissassembleUpdateGlobal(std::size_t &i);

private:
  std::vector<std::size_t> lines_; // TODO: more efficient storage strategy
  std::unordered_map<std::string, std::size_t> global_to_index_;
};

#endif // !PROGRAM_H
