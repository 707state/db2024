#define private public
#include "index/ix_manager.h"
#include "record/rm_manager.h"
#include "storage/buffer_pool_manager.h"
#include "storage/disk_manager.h"
#include "system/sm_manager.h"
#include "type/type_id.h"
#include <binder/statement/create_statement.h>
#include <binder/statement/drop_statement.h>

#include <binder/binder.h>
#undef private
#include <exception>
#include <memory>
#include <vector>

#include <gtest/gtest.h>
auto disk_manager = std::make_unique<DiskManager>();
auto bpm = std::make_unique<BufferPoolManager>(10, disk_manager.get());
auto indm = std::make_unique<IxManager>(disk_manager.get(), bpm.get());
auto rm = std::make_unique<RmManager>(disk_manager.get(), bpm.get());
auto sm_manager = std::make_unique<SmManager>(disk_manager.get(), bpm.get(),
                                              rm.get(), indm.get());
TEST(CreateDBTest, SimpleTest) {
  sm_manager->create_db("GRAGE");
  EXPECT_EQ(disk_manager->is_dir("GRAGE"), 1);
  sm_manager->drop_db("GRAGE");
  EXPECT_NE(disk_manager->is_dir("GRAGE"), 1);
}
TEST(CreateTableTest, SimpleTest) {
  try {
    sm_manager->create_db("GRADE");
  } catch (const std::exception &e) {
    std::cerr << e.what();
  }
  std::vector<ColDef> defs{ColDef{"id", TypeId::INTEGER, sizeof(int)}};
  sm_manager->create_table("name", defs, nullptr);
  EXPECT_EQ(sm_manager->db_.get_table("name").cols().size(), 1);
  EXPECT_EQ(sm_manager->db_.tabs_.size(), 1);
  sm_manager->drop_table("name", nullptr);
}
TEST(CreateTableTest, ComplexTest) {
  try {
    sm_manager->create_db("GRADE");
  } catch (std::exception &e) {
    std::cerr << e.what();
  }
  std::vector<ColDef> defs{{"id", TypeId::INTEGER, sizeof(int)},
                           {"name", TypeId::VARCHAR, 20},
                           {"character", TypeId::DECIMAL, sizeof(double)}};
  sm_manager->create_table("person", defs, nullptr);
  EXPECT_EQ(sm_manager->db_.get_table("person").cols().size(), 3);
  std::vector<ColDef> defs1{{"id1", TypeId::INTEGER, sizeof(int)},
                            {"name1", TypeId::VARCHAR, 20},
                            {"character2", TypeId::DECIMAL, sizeof(double)}};

  sm_manager->add_table_cols("person", defs1, nullptr);
  EXPECT_EQ(sm_manager->db_.get_table("person").cols().size(), 6);
}
