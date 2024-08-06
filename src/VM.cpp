#include "VM.h"
#include "VortexTypes.h"
#include <cassert>
#include <iostream>

VM::VM(Program &bytecode) : bytecode_{bytecode} {}

auto VM::run() -> VMState {
  while (state_ == VMState::OK) {
    state_ = executeOp();
  }
  switch (state_) {
  case VMState::COMPILE_ERR:
    break;
  case VMState::RUNTIME_ERR:
    std::cerr << error_ << "\n";
    break;
  case VMState::STACK_OVERFLOW:
    std::cerr << "Stack overflow!\n";
    break;
  case VMState::STACK_UNDERFLOW:
    std::cerr << "Stack underflow!\n";
    break;
  case VMState::HALTED:
    std::cout << stack_[stack_top_].Value.AsDouble << "\n";
    break;
  default:
    break;
  }

  if (state_ != VMState::HALTED) {
    std::cout << "Would you like to print the stack? (y/n)";
    auto c = char{};
    std::cin >> c;
    if (c == 'y') {
      printStack();
    }
  }

  return state_;
}

auto VM::executeOp() -> VMState {
  switch (bytecode_.Bytecode[PC_]) {
  case POP:
    if (!check(VMState::STACK_UNDERFLOW, 1)) {
      state_ = VMState::STACK_UNDERFLOW;
      return state_;
    }
    pop();
    break;
  case PUSHC:
    pushc();
    break;
  case ADD:
    if (!check(VMState::STACK_UNDERFLOW, 2)) {
      state_ = VMState::STACK_UNDERFLOW;
      return state_;
    }
    add();
    break;
  case SUB:
    if (!check(VMState::STACK_UNDERFLOW, 2)) {
      state_ = VMState::STACK_UNDERFLOW;
      return state_; // we return early so printing the stack shows which instr
                     // failed.
    }
    sub();
    break;
  case HALT:
    state_ = VMState::HALTED;
    std::cout << stack_[stack_top_ - 1].Value.AsDouble << "\n";
    break;
  default:
    state_ = VMState::RUNTIME_ERR;
    error_ = "Invalid instruction!";
    break;
  }
  ++PC_;
  return state_;
}

auto VM::pushc() -> void {
  assert((PC_ + 1 < bytecode_.Bytecode.size()) &&
         "No operand for instruction PUSHC!");
  PC_++;
  auto lookup_index = static_cast<std::size_t>(bytecode_.Bytecode[PC_]);
  auto constant = bytecode_.getConstant(lookup_index);
  push(constant);
}

auto VM::pop() -> VortexValue {
  --stack_top_;              // shrink the stack
  return stack_[stack_top_]; // return the element
}

auto VM::push(VortexValue value) -> void {
  if (!check(VMState::STACK_OVERFLOW)) {
    state_ = VMState::HALTED;
    return;
  }
  stack_[stack_top_] = value;
  ++stack_top_;
}

auto VM::add() -> void {
  auto b = pop();
  auto a = pop();
  // TODO: type checking
  push(VortexValue{.Type = ValueType::DOUBLE,
                   .Value = {a.Value.AsDouble + b.Value.AsDouble}});
}

auto VM::sub() -> void {
  auto b = pop();
  auto a = pop();
  // TODO: type checking
  push(VortexValue{.Type = ValueType::DOUBLE,
                   .Value = {a.Value.AsDouble - b.Value.AsDouble}});
}

auto VM::check(VMState for_state, std::size_t expected_size) -> bool {
  assert((for_state == VMState::STACK_OVERFLOW ||
          for_state == VMState::STACK_UNDERFLOW) &&
         "Cannot check for unknowable states.");
  switch (for_state) {
  case VMState::STACK_OVERFLOW:
    if (stack_top_ >= STACK_SIZE_) {
      return false;
    }
    return true;
  case VMState::STACK_UNDERFLOW:
    if (stack_top_ < expected_size) {
      return false;
    }
    return true;
  default:
    // you're never getting here (hopefully)
    return true;
  }
}

auto VM::printStack() -> void {
  std::cout << "STACK BOTTOM is here. \n";
  std::cout << "Program counter: " << PC_ << "\n";
  for (std::size_t i = 0; i < stack_top_; ++i) {
    switch (stack_[i].Type) {
    case ValueType::DOUBLE:
      std::cout << "DOUBLE: " << stack_[i].Value.AsDouble << "\n";
    }
  }
}
