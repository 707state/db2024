#pragma once

#include "common/context.h"
#include "common/err_message.h"
#include "index/ix.h"
#include "record/rm_file_handle.h"
#include "sm_defs.h"
#include "sm_meta.h"
#include <optional>
class Context;

struct ColDef {
  std::string name; // 该列的名字
  ColType type;     // 该列的类型
  int len;          // 该列的长度
};
// 考虑到现有框架对于修改元数据并不友好，在此进行一些修改
/* 系统管理器，负责元数据管理和DDL语句的执行 */
class SmManager {
public:
  DbMeta db_; // 当前打开的数据库的元数据
  std::unordered_map<std::string, std::unique_ptr<RmFileHandle>>
      fhs_; // file name -> record file handle, 当前数据库中每张表的数据文件
  std::unordered_map<std::string, std::unique_ptr<IxIndexHandle>>
      ihs_; // file name -> index file handle, 当前数据库中每个索引的文件
private:
  DiskManager *disk_manager_;
  BufferPoolManager *buffer_pool_manager_;
  RmManager *rm_manager_;
  IxManager *ix_manager_;

public:
  SmManager(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager,
            RmManager *rm_manager, IxManager *ix_manager)
      : disk_manager_(disk_manager), buffer_pool_manager_(buffer_pool_manager),
        rm_manager_(rm_manager), ix_manager_(ix_manager) {}

  ~SmManager() {}

  BufferPoolManager *get_bpm() { return buffer_pool_manager_; }

  RmManager *get_rm_manager() { return rm_manager_; }

  IxManager *get_ix_manager() { return ix_manager_; }
  RC_VALUES is_dir_rc(const std::string &db_name);
  bool is_dir(const std::string &db_name);
  RC_VALUES create_db_rc(const std::string &db_name);
  void create_db(const std::string &db_name);
  RC_VALUES drop_db_rc(const std::string &db_name);
  void drop_db(const std::string &db_name);
  RC_VALUES open_db_rc(const std::string &db_name);
  void open_db(const std::string &db_name);
  RC_VALUES close_db_rc();
  void close_db();
  RC_VALUES flush_meta_rc();
  void flush_meta();
  void show_tables(Context *context);
  void desc_table(const std::string &tab_name, Context *context);
  RC_VALUES create_table_rc(const std::string &tab_name,
                            const std::vector<ColDef> &col_def,
                            Context *context);
  void create_table(const std::string &tab_name,
                    const std::vector<ColDef> &col_defs, Context *context);

  std::optional<int> add_table_cols(const std::string &tab_name,
                                    const std::vector<ColDef> &col_defs,
                                    Context *context);
  RC_VALUES drop_table_rc(const std::string &tab_name, Context *context);
  void drop_table(const std::string &tab_name, Context *context);
  RC_VALUES create_index_rc(const std::string &tab_name,
                            const std::vector<std::string> &col_names,
                            Context *context);
  void create_index(const std::string &tab_name,
                    const std::vector<std::string> &col_names,
                    Context *context);
  RC_VALUES drop_index_rc(const std::string &tab_name,
                          const std::vector<std::string> &col_names,
                          Context *context);
  void drop_index(const std::string &tab_name,
                  const std::vector<std::string> &col_names, Context *context);
  RC_VALUES drop_index_rc(const std::string &tab_name,
                          const std::vector<ColMeta> &col_names,
                          Context *context);
  void drop_index(const std::string &tab_name,
                  const std::vector<ColMeta> &col_names, Context *context);
};
