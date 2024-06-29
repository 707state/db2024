#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_statement.h"
#include "binder/statement/create_statement.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/explain_statement.h"
#include "binder/statement/index_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/statement/update_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "common/exception.h"
#include "common/logger.h"
#include "common/util/string_util.h"
#include "nodes/nodes.hpp"
#include "nodes/parsenodes.hpp"
#include "type/decimal_type.h"
#include <memory>

void Binder::SaveParseTree(duckdb_libpgquery::PGList *tree) {
  std::vector<std::unique_ptr<BoundStatement>> statements;
  for (auto entry = tree->head; entry != nullptr; entry = entry->next) {
    statement_nodes_.push_back(
        reinterpret_cast<duckdb_libpgquery::PGNode *>(entry->data.ptr_value));
  }
}

auto Binder::BindStatement(duckdb_libpgquery::PGNode *stmt)
    -> std::unique_ptr<BoundStatement> {
  switch (stmt->type) {
  case duckdb_libpgquery::T_PGRawStmt:
    return BindStatement(
        reinterpret_cast<duckdb_libpgquery::PGRawStmt *>(stmt)->stmt);
  case duckdb_libpgquery::T_PGCreateStmt:
    return BindCreate(
        reinterpret_cast<duckdb_libpgquery::PGCreateStmt *>(stmt));
  case duckdb_libpgquery::T_PGInsertStmt:
    return BindInsert(
        reinterpret_cast<duckdb_libpgquery::PGInsertStmt *>(stmt));
  case duckdb_libpgquery::T_PGSelectStmt:
    return BindSelect(
        reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(stmt));
  case duckdb_libpgquery::T_PGExplainStmt:
    return BindExplain(
        reinterpret_cast<duckdb_libpgquery::PGExplainStmt *>(stmt));
  case duckdb_libpgquery::T_PGDeleteStmt:
    return BindDelete(
        reinterpret_cast<duckdb_libpgquery::PGDeleteStmt *>(stmt));
  case duckdb_libpgquery::T_PGUpdateStmt:
    return BindUpdate(
        reinterpret_cast<duckdb_libpgquery::PGUpdateStmt *>(stmt));
  case duckdb_libpgquery::T_PGIndexStmt:
    return BindIndex(reinterpret_cast<duckdb_libpgquery::PGIndexStmt *>(stmt));
  case duckdb_libpgquery::T_PGVariableSetStmt:
    return BindVariableSet(
        reinterpret_cast<duckdb_libpgquery::PGVariableSetStmt *>(stmt));
  case duckdb_libpgquery::T_PGVariableShowStmt:
    return BindVariableShow(
        reinterpret_cast<duckdb_libpgquery::PGVariableShowStmt *>(stmt));
  default:
    throw NotImplementedException(NodeTagToString(stmt->type));
  }
}
