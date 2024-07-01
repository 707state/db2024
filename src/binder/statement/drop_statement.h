#pragma once
#include "binder/bound_statement.h"
#include "common/enums/statement_type.h"
#include <algorithm>
#include <string>
#include <sys/types.h>
struct PGDropStmt;
enum class DropType : u_int8_t { INVALID, TABLE, DB, INDEX };
class DropStatement : public BoundStatement {
public:
  DropStatement(std::string tab_name, DropType type)
      : BoundStatement(StatementType::DROP_STATEMENT),
        drop_from(std::move(tab_name)), type(type) {}
  std::string drop_from; // 要被drop
                         // statement接受的字符串，有table/database/index
  DropType type = DropType::INVALID;
  auto ToString() const -> std::string;
};
