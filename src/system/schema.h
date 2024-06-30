#pragma once
#include "common/err_message.h"

#include "fmt/base.h"
#include "fmt/core.h"
#include "system/col_meta.h"
#include "system/column.h"
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
class Schema {
public:
  Schema() = default;
  Schema(const std::vector<Column> &columns, const std::string &tab_name);
  // Schema(const std::vector<Column> &columns);
  // Schema(const std::vector<ColMeta> &column_metas);
  Schema(std::vector<ColMeta> columns) : schema_cols_(std::move(columns)) {}
  Schema(std::vector<Column> columns) {
    size_t curr_offset = 0;
    for (auto index = 0; index < columns.size(); index++) {
      auto column = columns[index];
      if (!column.is_inlined()) {
        tuple_is_inlined_ = false;
        schema_uninlined_cols_.push_back(index);
      }
      column.set_offset(curr_offset);
      if (column.is_inlined()) {
        curr_offset += column.get_len();
      } else {
        // 几种数据的长度不一致。
        curr_offset += column.get_len();
      }
      schema_cols_.push_back(ColMeta{this->tab_name.value(), column.get_name(),
                                     column.get_type(),
                                     static_cast<int>(column.get_len())});
    }
    schema_len_ = curr_offset;
  }
  static Column colmeta_to_column(const ColMeta &col_meta) {
    return Column{col_meta.name, col_meta.type,
                  static_cast<size_t>(col_meta.len),
                  static_cast<size_t>(col_meta.offset)};
  }
  static ColMeta column_to_colmeta(const Column &column) {
    return ColMeta{"", column.get_name(), column.get_type(),
                   static_cast<int>(column.get_len())};
  }
  static auto copy_schema(const Schema *from,
                          const std::vector<size_t> &attrs) -> Schema {
    std::vector<Column> cols;
    cols.reserve(attrs.size());
    for (const auto i : attrs) {
      cols.push_back(colmeta_to_column(from->schema_cols_[i]));
    }
    return Schema{cols};
  }
  auto colmeta_wrapper(const ColMeta &col_meta) {
    return Column{col_meta.name, col_meta.type,
                  static_cast<size_t>(col_meta.len),
                  static_cast<size_t>(col_meta.offset)};
  }
  auto colmeta_wrapper(ColMeta &&col_meta) {
    return Column{col_meta.name, col_meta.type,
                  static_cast<size_t>(col_meta.len),
                  static_cast<size_t>(col_meta.offset)};
  }
  [[nodiscard]] auto get_columns() const -> const std::vector<Column> & {
    std::vector<Column> cols;
    for (auto &c : schema_cols_) {
      cols.push_back(Column{c.name, c.type, static_cast<size_t>(c.len),
                            static_cast<size_t>(c.offset)});
    }
    return cols;
  }
  auto set_tab(const std::string &tan) { this->tab_name = tan; }
  auto set_tab(std::string &&tan) { this->tab_name = std::move(tan); }
  [[nodiscard]] auto get_colmetas() const -> const std::vector<ColMeta> & {
    return schema_cols_;
  }
  auto get_column(const int col_idx) const -> const Column & {
    auto tmp = schema_cols_[col_idx];
    return Column{tmp.name, tmp.type, static_cast<size_t>(tmp.len),
                  static_cast<size_t>(tmp.offset)};
  }
  auto column_push_back(const Column &col) {
    this->schema_cols_.push_back(ColMeta{this->tab_name.value(), col.get_name(),
                                         col.get_type(),
                                         static_cast<int>(col.get_len())});
  }
  auto column_push_back(const ColMeta &col_meta) {
    this->schema_cols_.push_back(col_meta);
  }
  auto get_col_idx(const std::string &col_name) {
    if (auto res = try_get_col_idx(col_name)) {
      return *res;
    }
    LOG_WARNING("Column: %s does not exist.\n", col_name.c_str());
  }
  std::optional<int> try_get_col_idx(const std::string &col_name) {
    for (auto i = 0; i < schema_cols_.size(); i++) {
      if (schema_cols_[i].name == col_name) {
        return std::optional{i};
      }
    }
    return std::nullopt;
  }
  auto get_uninlined_column() const { return schema_uninlined_cols_; }
  auto get_column_size() const { return schema_cols_.size(); }
  auto get_uninlined_column_size() const {
    return schema_uninlined_cols_.size();
  }
  inline auto getget_inlined_storaged_size() const { return schema_len_; }
  inline auto inlines() const { return tuple_is_inlined_; }
  auto to_string(bool simplified = true) const -> std::string;
  std::optional<std::string> tab_name;
  size_t schema_len_;
  std::vector<ColMeta> schema_cols_;
  bool tuple_is_inlined_{true};
  std::vector<size_t> schema_uninlined_cols_;
};

template <typename T>
struct fmt::formatter<T,
                      std::enable_if_t<std::is_base_of<Schema, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const Schema &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.to_string(), ctx);
  }
};
template <typename T>
struct fmt::formatter<std::shared_ptr<T>,
                      std::enable_if_t<std::is_base_of<Schema, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::shared_ptr<Schema> &x, FormatCtx &ctx) const {
    return x ? fmt::formatter<std::string>(x->to_string(), ctx)
             : fmt::formatter<std::string>("", ctx);
  }
};
template <typename T>
struct fmt::formatter<std::unique_ptr<T>,
                      std::enable_if_t<std::is_base_of<Schema, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<Schema> &x, FormatCtx &ctx) const {
    return x ? fmt::formatter<std::string>(x->to_string(), ctx)
             : fmt::formatter<std::string>("", ctx);
  }
};
