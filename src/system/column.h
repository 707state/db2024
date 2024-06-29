#pragma once

#include "common/err_message.h"
#include "defs.h"
#include "fmt/base.h"
#include "fmt/core.h"
#include "type/type_id.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>
class ColMeta;
class Column {
public:
  // explicit Column(ColMeta a)
  //     : col_name_(std::move(a.name)), type_id(a.type), col_len_(a.len),
  //       col_offset_(a.offset) {}
  Column() = default;
  Column(const Column &) = default;
  Column(Column &&) = default;
  Column &operator=(const Column &) = default;
  Column &operator=(Column &&) = default;
  Column(std::string col_name, TypeId col_type)
      : col_name_(std::move(col_name)), type_id(col_type),
        col_len_(TypeSize(col_type)) {
    if (type_id == TypeId::VARCHAR) {
      LOG_WARNING("column: %s.\tstring type must have length.\n",
                  col_name_.c_str());
    }
  }
  Column(std::string col_name, TypeId col_type, size_t col_len)
      : col_name_(std::move(col_name)), type_id(col_type), col_len_(col_len) {
    if (type_id != TypeId::VARCHAR) {
      LOG_WARNING("column: %s\tonly string type needs length.\n",
                  col_name_.c_str());
    }
  }
  Column(std::string col_name, ColType type, size_t col_len)
      : col_name_(std::move(col_name)), col_len_(col_len) {

    switch (type) {
    case ColType::TYPE_FLOAT: {
      type_id = TypeId::DECIMAL;
      break;
    }
    case ColType::TYPE_INT: {
      type_id = TypeId::SMALLINT;
      break;
    }
    case ColType::TYPE_STRING: {
      type_id = TypeId::VARCHAR;
      break;
    }
    case ColType::TYPE_TIMESTAMP: {
      type_id = TypeId::TIMESTAMP;
      break;
    }
    default: {
      type_id = TypeId::INVALID;
    }
    }
  }
  // inline Column(ColMeta col_meta)
  //     : col_name_(col_meta.name), col_len_(col_meta.len),
  //       type_id(col_meta.type), col_offset_(col_meta.offset) {}
  Column(std::string col_name, TypeId col_type, size_t col_len, size_t offset)
      : col_name_(std::move(col_name)), type_id(col_type), col_len_(col_len),
        col_offset_(offset) {
    if (type_id != TypeId::VARCHAR) {
      LOG_WARNING("column: %s\tonly string type needs length.\n",
                  col_name_.c_str());
    }
  }
  static auto TypeSize(TypeId type) -> uint8_t {
    switch (type) {
    case TypeId::BOOLEAN:
    case TypeId::TINYINT:
      return 1;
    case TypeId::SMALLINT:
      return 2;
    case TypeId::INTEGER:
      return 4;
    case TypeId::BIGINT:
    case TypeId::DECIMAL:
    case TypeId::TIMESTAMP:
      return 8;
    case TypeId::VARCHAR:
      // TODO(Amadou): Confirm this.
      return 12;
    default: {
      return 0;
    }
    }
  }

  auto setLen(int len) -> Column * {
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
  bool is_inlined() const { return type_id != TypeId::VARCHAR; }
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
  TypeId type_id;
  size_t col_len_{};
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
