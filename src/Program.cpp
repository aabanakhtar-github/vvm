#include "Program.h"
#include "VortexTypes.h"
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>

static constexpr auto UINT24_MAX = 16'777'215;

auto Program::pushCode(std::uint8_t code, std::size_t line) -> std::size_t {
  Bytecode.push_back(code);
  lines_.push_back(line);
  return Bytecode.size() - 1;
}

auto Program::addConstant(VortexValue constant) -> std::int32_t {
  if (Constants.size() >= UINT24_MAX) {
    return -1;
  }
  Constants.push_back(constant);
  return Constants.size() - 1;
}

auto Program::createString(std::string_view contents) -> Object * {
  auto index = Objects.size();
  Objects.emplace_back(std::make_unique<StringObject>(std::string{contents}));
  Objects.back()->Type = ObjectType::STR;
  return Objects.back().get();
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
  // TODO: just change the name to something
  // like "line" or "instruction"
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
  case NOT:
    ++i;
    return "NOT";
  case EQ:
    ++i;
    return "EQ";
  case LESS_EQ:
    ++i;
    return "LESS_EQ";
  case GREATER_EQ:
    ++i;
    return "GREATER_EQ";
  case GREATER:
    ++i;
    return "GREATER";
  case LESS:
    ++i;
    return "LESS";
  case PUSH_NIL:
    ++i;
    return "PUSH_NIL";
  case PUSH_TRUE:
    ++i;
    return "PUSH_TRUE";
  case PUSH_FALSE:
    ++i;
    return "PUSH_FALSE";
  case PRINT:
    ++i;
    return "PRINT";
  case SAVE_GLOB:
    ++i;
    return "STORE_GLOB";
  case LOAD_GLOB:
    ++i;
    return "LOAD_GLOB";
  case ADD_LOCAL:
    ++i;
    return "ADD_LOCAL";
  case SET_LOCAL:
    ++i;
    return "SET_LOCAL";
  case GET_LOCAL:
    ++i;
    return "GET_LOCAL";
  case JMP_TO:
    ++i;
    return "JMP_TO";
  case JMP_TO_IF_FALSE:
    // TODO: fix
    ++i;
    return "JMP_TO_IF_FALSE";
  }
  return "";
}

auto Program::dissassembleConstant(std::size_t &i) -> std::string {
  assert(i + 3 < Bytecode.size() &&
         "Not enought bytecode to disassemble push constant instruction");
  auto b1 = static_cast<std::int32_t>(
      Bytecode[i + 1]); // the three bytes of the instruction
  auto b2 = static_cast<std::int32_t>(Bytecode[i + 2]);
  auto b3 = static_cast<std::int32_t>(Bytecode[i + 3]);
  auto constant_index = (b1 << 16) | (b2 << 8) | (b3);
  auto instr = "PUSHC " + std::to_string(constant_index);

  i += 4; // 4 byte instruction (instr b0, b1, b2)
  return instr + "\n extra byte \n extra byte \n extra byte";
}

auto Program::createGlobal(std::string_view name,
                           VortexValue val) -> std::size_t {
  auto index = Globals.size();
  Globals.push_back(val);
  assert(global_to_index_.contains(std::string{name}) == false &&
         "Code Generation Error: Cannot create already defined global.");
  global_to_index_[std::string{name}] = index;
  return index;
}
