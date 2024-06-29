/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL
v2. You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
// #define DEBUG
#include "common/config.h"
#include "errors.h"
#include <common/err_message.h>

/**
 * @description: DiskManager的作用主要是根据上层的需要对磁盘文件进行操作
 */
class DiskManager {
public:
  explicit DiskManager();

  ~DiskManager() = default;
  RC_VALUES write_page_rc(int fd, rmdb::page_id_t page_no, const char *offset,
                          int num_bytes);
  void write_page(int fd, rmdb::page_id_t page_no, const char *offset,
                  int num_bytes);
  RC_VALUES read_page_rc(int fd, rmdb::page_id_t page_no, char *offset,
                         int num_bytes);
  void read_page(int fd, rmdb::page_id_t page_no, char *offset, int num_bytes);

  rmdb::page_id_t allocate_page(int fd);
  RC_VALUES deallocate_page_rc(rmdb::page_id_t page_id);
  void deallocate_page(rmdb::page_id_t page_id);

  /*目录操作*/
  auto is_dir(const std::string &path) -> bool;
  RC_VALUES create_dir_rc(const std::string &path);
  void create_dir(const std::string &path);
  RC_VALUES destroy_dir_rc(const std::string &path);
  void destroy_dir(const std::string &path);

  /*文件操作*/
  auto is_file(const std::string &path) -> bool;
  RC_VALUES create_file_rc(const std::string &path);
  void create_file(const std::string &path);
  RC_VALUES destroy_file_rc(const std::string &path);
  void destroy_file(const std::string &path);

  auto open_file(const std::string &path) -> int;
  RC_VALUES close_file_rc(int fd);
  void close_file(int fd);

  auto get_file_size(const std::string &file_name) -> int;

  auto get_file_name(int fd) -> std::string;

  auto get_file_fd(const std::string &file_name) -> int;

  /*日志操作*/
  auto read_log(char *log_data, int size, int offset) -> int;
  // RC_VALUES write_log_rc(char *log_data, int size);
  void write_log(char *log_data, int size);
  void SetLogFd(int log_fd) { log_fd_ = log_fd; }

  auto GetLogFd() -> int { return log_fd_; }

  /**
   * @description: 设置文件已经分配的页面个数
   * @param {int} fd 文件对应的文件句柄
   * @param {int} start_page_no
   * 已经分配的页面个数，即文件接下来从start_page_no开始分配页面编号
   */
  void set_fd2pageno(int fd, int start_page_no) {
    fd2pageno_[fd] = start_page_no;
  }

  /**
   * @description:
   * 获得文件目前已分配的页面个数，即如果文件要分配一个新页面，需要从fd2pagenp_[fd]开始分配
   * @return {page_id_t} 已分配的页面个数
   * @param {int} fd 文件对应的句柄
   */
  auto get_fd2pageno(int fd) -> rmdb::page_id_t { return fd2pageno_[fd]; }

  static constexpr int MAX_FD = 8192;

private:
  // 文件打开列表，用于记录文件是否被打开
  std::unordered_map<std::string, int>
      path2fd_; //<Page文件磁盘路径,Page fd>哈希表
  std::unordered_map<int, std::string>
      fd2path_; //<Page fd,Page文件磁盘路径>哈希表

  int log_fd_ = -1; // WAL日志文件的文件句柄，默认为-1，代表未打开日志文件
  std::atomic<rmdb::page_id_t>
      fd2pageno_[MAX_FD]{}; // 文件中已经分配的页面个数，初始值为0
};
