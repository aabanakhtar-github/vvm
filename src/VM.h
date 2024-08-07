#ifndef VM_H
#define VM_H

#include "Program.h"
#include "VortexTypes.h"
#include <array>

enum class VMState {
  OK,
  HALTED,
  STACK_OVERFLOW,
  STACK_UNDERFLOW,
  COMPILE_ERR,
  RUNTIME_ERR
};

class VM {
public:
  explicit VM(Program &bytecode);
  auto run() -> VMState;

private:
  auto executeOp() -> VMState;
  // instructions implementations
  auto pushc() -> void; // loads a constant
  auto push(VortexValue value) -> void;
  auto pop() -> VortexValue;
  auto add() -> void;
  auto sub() -> void;
  auto mul() -> void;
  auto div() -> void;
  auto negate() -> void;
  auto eq() -> void;
  auto lessThanOrEqual() -> void;
  auto greaterThanOrEqual() -> void;
  auto greater() -> void;
  auto less() -> void;
  // safety
  // returns true if the state is ok.
  auto check(VMState for_state, std::size_t expected_size = 0) -> bool;

  // util
  auto printStack() -> void;

private:
  static constexpr std::size_t STACK_SIZE_ = 2048;
  std::size_t PC_ = 0;
  std::size_t stack_top_ = 0;
  VMState state_ = VMState::OK;
  Program &bytecode_;
  std::array<VortexValue, STACK_SIZE_> stack_;
  std::string error_;
};

#endif // !VM_H
