#pragma once

#include "common/err_message.h"
#include "defs.h"
#include "fmt/base.h"
#include "fmt/core.h"
#include "system/col_meta.h"
#include "type/type_id.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
namespace rmdb {
class ColMeta;
class Column {
public:
  // explicit Column(ColMeta a)
  //     : col_name_(std::move(a.name)), type_id(a.type), col_len_(a.len),
  //       col_offset_(a.offset) {}
  Column() = default;
  Column(std::string col_name, ColType col_type)
      : col_name_(std::move(col_name)), type_id(col_type),
        col_len_(coltype2len(col_type)) {
    if (type_id == ColType::TYPE_STRING) {
      LOG_WARNING("column: %s.\tstring type must have length.\n",
                  col_name_.c_str());
    }
  }
  Column(std::string col_name, ColType col_type, size_t col_len)
      : col_name_(std::move(col_name)), type_id(col_type), col_len_(col_len) {
    if (type_id != ColType::TYPE_STRING) {
      LOG_WARNING("column: %s\tonly string type needs length.\n",
                  col_name_.c_str());
    }
  }
  // inline Column(ColMeta col_meta)
  //     : col_name_(col_meta.name), col_len_(col_meta.len),
  //       type_id(col_meta.type), col_offset_(col_meta.offset) {}
  Column(std::string col_name, ColType col_type, size_t col_len, size_t offset)
      : col_name_(std::move(col_name)), type_id(col_type), col_len_(col_len),
        col_offset_(offset) {
    if (type_id != ColType::TYPE_STRING) {
      LOG_WARNING("column: %s\tonly string type needs length.\n",
                  col_name_.c_str());
    }
  }
  Column *setLen(int len) {
    this->col_len_ = len;
    return this;
  }
  Column(std::string col_name, const Column &other)
      : col_name_(std::move(col_name)), type_id(other.type_id),
        col_len_(other.col_len_), col_offset_(other.col_offset_) {}
  [[nodiscard]] auto get_name() const { return col_name_; }
  [[nodiscard]] auto get_type() const { return type_id; }
  [[nodiscard]] auto get_len() const { return col_len_; }
  [[nodiscard]] auto get_offset() const { return col_offset_; }
  auto set_offset(size_t off) { this->col_offset_ = off; }
  auto th_column_name(std::string col_name) -> Column {
    Column c = *this;
    c.col_name_ = std::move(col_name);
    return c;
  }
  bool is_inlined() const { return type_id != ColType::TYPE_STRING; }
  [[nodiscard]] auto to_string(bool simplified = true) const -> std::string;
  friend std::ostream &operator<<(std::ostream &os, const Column &col) {
    return os << col.to_string();
  }
  Column *setErrorMsg(std::string msg) {
    this->err_msg = std::move(msg);
    return this;
  }
  std::optional<std::string> getErrorMsg() {
    if (err_msg.size()) {
      return err_msg;
    }
    return std::nullopt;
  }

private:
  std::string col_name_;
  ColType type_id;
  size_t col_len_;
  size_t col_offset_{};

private:
  std::string err_msg{""};
};

template <typename T>
struct fmt::formatter<T,
                      std::enable_if_t<std::is_base_of<Column, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const Column &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.to_string(), ctx);
  }
};
template <typename T>
struct fmt::formatter<std::unique_ptr<T>,
                      std::enable_if_t<std::is_base_of<Column, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename F>
  auto format(const std::unique_ptr<Column> &x, F &ctx) const {
    return fmt::formatter<std::string>::format(x->to_string(), ctx);
  }
};
} // namespace rmdb
