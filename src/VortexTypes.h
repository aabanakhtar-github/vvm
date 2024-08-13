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
  NOT,        // !
  NEGATE,     // - (pop())
  EQ,         // ==
  LESS_EQ,    // <=
  GREATER_EQ, // >=
  GREATER,    // >
  LESS,       // <
  HALT,
  INVALID_OP
};

enum class ValueType : std::uint8_t { DOUBLE, BOOL, NIL };
struct Object {};

struct VortexValue {
  union Value {
    double AsDouble;
    bool AsBool;
    Object *AsObject;
  };

  ValueType Type;
  Value Value;

  auto asString() -> std::string {
    switch (Type) {
    case ValueType::DOUBLE:
      return std::to_string(Value.AsDouble);
      break;
    case ValueType::BOOL:
      return Value.AsBool == true ? "true" : "false";
      break;
    case ValueType::NIL:
      return "nil";
      break;
    }
  }
};

#endif // !VORTEX_TYPES_H
