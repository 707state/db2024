#include "common/exception.h"
#include <exception>
#include <iostream>
#include <utility>
#include <vector>
#define private public
#include "binder/binder.h"
#include "binder/bound_statement.h"
#include "type/type_id.h"

#include "record/rm_manager.h"
#include "system/sm_manager.h"
#undef private
#include "gtest/gtest.h"
#include <memory>

#include <string>
using fmt::format;

constexpr int MAX_FILES = 32;
constexpr int MAX_PAGES = 128;
constexpr size_t TEST_BUFFER_POOL_SIZE = MAX_FILES * MAX_PAGES;
const std::string TEST_DB_NAME =
    "BufferPoolManagerTest_db"; // 以TEST_DB_NAME作为存放测试文件的根目录名

auto disk_manager = std::make_unique<DiskManager>();
auto buffer_pool_manager = std::make_unique<BufferPoolManager>(
    TEST_BUFFER_POOL_SIZE, disk_manager.get());
auto rm_manager =
    std::make_unique<RmManager>(disk_manager.get(), buffer_pool_manager.get());
auto ix_manager =
    std::make_unique<IxManager>(disk_manager.get(), buffer_pool_manager.get());
auto sm_manager =
    std::make_unique<SmManager>(disk_manager.get(), buffer_pool_manager.get(),
                                rm_manager.get(), ix_manager.get());
std::unordered_map<int, char *> mock;
auto binder = std::make_unique<Binder>(sm_manager.get());
auto TryBind(const std::string query) {
  binder->ParseAndSave(query);
  std::vector<std::unique_ptr<BoundStatement>> statements;
  for (auto *stmt : binder->statement_nodes_) {
    auto statement = binder->BindStatement(stmt);
    statements.emplace_back(std::move(statement));
  }
  return statements;
}
void PrintStatements(
    const std::vector<std::unique_ptr<BoundStatement>> &statements) {
  for (const auto &statement : statements) {
    std::cout << statement->ToString() << std::endl;
  }
}

TEST(BinderTest, BindSelectValue) {
  auto statements = TryBind("select 1+2*3");
  PrintStatements(statements);
}
TEST(BinderTest, BindCreateTable) {
  try {
    auto statements = TryBind("create table x (name int4)");
    PrintStatements(statements);
  } catch (const std::exception &e_) {
    std::cout << format("Can not create table {}: Reason: {}", "x", e_.what());
  }
}
TEST(BinderTest, NewTable) {

  std::vector<ColDef> cols;
  cols.push_back(ColDef{"name", TypeId::INTEGER});
  cols.push_back(ColDef{"id", TypeId::INTEGER});
  try {
    binder->sm_manager_->create_table("z", cols, nullptr);
    auto statements = TryBind("select * from z");
    PrintStatements(statements);
  } catch (const std::exception &e) {
    std::cout << e.what();
  }
}
TEST(BinderTest, InsertTable) {
  try {
    auto statementss = TryBind("insert into z values (1,1)");
    PrintStatements(statementss);
  } catch (const std::exception &e_) {
    std::cout << format("Can not create table {}: Reason: {}", "x", e_.what());
  }
}
TEST(BInderTest, DeleteTable) {

  auto statements = TryBind("drop table z");
  PrintStatements(statements);
}
TEST(BinderTest, IndexStatement) {
  auto statements = TryBind("create index test_index on z(name);");
  PrintStatements(statements);
}
