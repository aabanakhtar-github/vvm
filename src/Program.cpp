#include "Program.h"
#include "VortexTypes.h"
#include <cassert>
#include <fstream>
#include <iostream>

auto Program::pushCode(std::uint8_t code, std::size_t line) -> void {
  Bytecode.push_back(code);
  lines_.push_back(line);
}

auto Program::addConstant(VortexValue constant) -> void {
  if (Constants.size() >= 255) {
    std::cerr << "Too many constants in one set of bytecode!\n";
    return;
  }
  Constants.push_back(constant);
}

auto Program::dissassemble(std::string_view output_filename) -> void {
  auto output_file = std::ofstream{std::string{output_filename} + ".vbyte",
                                   std::ios_base::out};
  // output all the constants first
  output_file << "CONSTANTS:\n";
  for (std::size_t i = 0; i < Constants.size(); ++i) {
    output_file << "[" << i << "] : " << Constants[i].asString() << "\n";
  }
  // then output the bytecode
  output_file << "BYTECODE BEGINS:\n";
  for (std::size_t i = 0; i < Bytecode.size();) {
    output_file << dissassembleInstruction(i) << "\n";
  }
  output_file.close();
}

auto Program::dissassembleInstruction(std::size_t &i) -> std::string {
  auto codename = std::to_string(lines_[i]) + ": ";
  switch (Bytecode[i]) {
  case PUSHC:
    codename += dissassembleConstant(i);
    break;
  default:
    codename += dissassembleRegular(i);
    break;
  }
  return codename;
}

auto Program::dissassembleRegular(std::size_t &i) -> std::string {
  switch (Bytecode[i]) {
  case ADD:
    ++i;
    return "ADD";
  case SUB:
    ++i;
    return "SUB";
  case HALT:
    ++i;
    return "HALT";
  case MUL:
    ++i;
    return "MUL";
  case DIV:
    ++i;
    return "DIV";
  case NEGATE:
    ++i;
    return "NEGATE";
  default:
    ++i;
    return "INVALID_OP";
  }
}

auto Program::dissassembleConstant(std::size_t &i) -> std::string {
  assert(i + 1 < Bytecode.size() &&
         "Not enought bytecode to disassemble constant instruction");
  auto constant = "PUSHC " + std::to_string(Bytecode[i + 1]);
  i += 2;
  return constant; // the second is index of
                   // the constant in the pool
}
