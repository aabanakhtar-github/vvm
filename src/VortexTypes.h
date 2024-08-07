#ifndef VORTEX_TYPES_H
#define VORTEX_TYPES_H

#include <cstdint>
#include <string>

enum OpCode : std::uint8_t {
  PUSHC = 0, // load constant
  POP,
  ADD,
  SUB,
  MUL,
  DIV,
  NEGATE,     // - (pop())
  EQ,         // ==
  LESS_EQ,    // <=
  GREATER_EQ, // >=
  GREATER,    // >
  LESS,       // <
  HALT,
  INVALID_OP
};

enum class ValueType : std::uint8_t { DOUBLE };

struct VortexValue {
  union Value {
    double AsDouble;
  };

  ValueType Type;
  Value Value;

  auto asString() -> std::string { return std::to_string(Value.AsDouble); }
};

#endif // !VORTEX_TYPES_H
