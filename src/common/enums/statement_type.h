
#pragma once

#include "common/config.h"
#include "fmt/format.h"

//===--------------------------------------------------------------------===//
// Statement Types
//===--------------------------------------------------------------------===//
enum class StatementType : uint8_t {
  INVALID_STATEMENT,       // invalid statement type
  SELECT_STATEMENT,        // select statement type
  INSERT_STATEMENT,        // insert statement type
  UPDATE_STATEMENT,        // update statement type
  CREATE_STATEMENT,        // create statement type
  DELETE_STATEMENT,        // delete statement type
  EXPLAIN_STATEMENT,       // explain statement type
  DROP_STATEMENT,          // drop statement type
  INDEX_STATEMENT,         // index statement type
  VARIABLE_SET_STATEMENT,  // set variable statement type
  VARIABLE_SHOW_STATEMENT, // show variable statement type
};

template <> struct fmt::formatter<StatementType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(StatementType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
    case StatementType::INVALID_STATEMENT:
      name = "Invalid";
      break;
    case StatementType::SELECT_STATEMENT:
      name = "Select";
      break;
    case StatementType::INSERT_STATEMENT:
      name = "Insert";
      break;
    case StatementType::UPDATE_STATEMENT:
      name = "Update";
      break;
    case StatementType::CREATE_STATEMENT:
      name = "Create";
      break;
    case StatementType::DELETE_STATEMENT:
      name = "Delete";
      break;
    case StatementType::EXPLAIN_STATEMENT:
      name = "Explain";
      break;
    case StatementType::DROP_STATEMENT:
      name = "Drop";
      break;
    case StatementType::INDEX_STATEMENT:
      name = "Index";
      break;
    case StatementType::VARIABLE_SHOW_STATEMENT:
      name = "VariableShow";
      break;
    case StatementType::VARIABLE_SET_STATEMENT:
      name = "VariableSet";
      break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
