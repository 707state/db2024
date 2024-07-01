#include "binder/binder.h"
#include "binder/statement/drop_statement.h"
#include "common/exception.h"
#include "common/macros.h"
#include "format.h"
#include "nodes/parsenodes.hpp"
#include "nodes/pg_list.hpp"
#include "nodes/value.hpp"
#include <algorithm>
#include <memory>
#include <nodes/nodes.hpp>
#include <string>

auto Binder::BindDrop(duckdb_libpgquery::PGDropStmt *pg_stmt)
    -> std::unique_ptr<DropStatement> {
  std::string relation{};
  DropType type{DropType::INVALID};
  switch (pg_stmt->type) {
  case duckdb_libpgquery::PG_OBJECT_INDEX: {
    type = DropType::INDEX;
    break;
  }
  case duckdb_libpgquery::PG_OBJECT_TABLE: {
    type = DropType::TABLE;
    break;
  }
  case duckdb_libpgquery::PG_OBJECT_DATABASE: {
    type = DropType::DB;
  }
  default: {
    throw Exception(fmt::format("Other drop type not implemented"));
  }
  }
  auto view_list = reinterpret_cast<duckdb_libpgquery::PGList *>(
      pg_stmt->objects->head->data.ptr_value);
  if (view_list->length > 1) {
    throw Exception(
        "Drop statement with arguments more than 1 is not implemented");
  }
  relation = reinterpret_cast<duckdb_libpgquery::PGValue *>(
                 view_list->head->data.ptr_value)
                 ->val.str;
  return std::make_unique<DropStatement>(std::move(relation), type);
}
