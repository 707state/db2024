#include "binder/binder.h"
#include "binder/bound_statement.h"
#include <type_traits>
#include <utility>
#include <vector>
#define private public
#include "record/rm_manager.h"
#include "system/sm_manager.h"
#undef private
#include "gtest/gtest.h"
#include <memory>

#include <string>
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
auto binder = std::make_unique<Binder>(std::move(sm_manager));
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
  auto statements = TryBind("select 1");
  PrintStatements(statements);
}
