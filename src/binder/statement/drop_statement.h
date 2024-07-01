#pragma once
#include "binder/bound_statement.h"
#include "common/enums/statement_type.h"
#include <string>
#include <vector>
struct PGDropStmt;
class DropStatement : public BoundStatement {
public:
  DropStatement(std::string tab_name)
      : BoundStatement(StatementType::DROP_STATEMENT) {}
  std::string tab_name_;
  // 如果是drop index
  std::string index_name;

  auto ToString() const -> std::string;
};
