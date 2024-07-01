#pragma once
#include "defs.h"
#include "fmt/base.h"
#include "fmt/format.h"
#include "system/column.h"
#include "type/type_id.h"
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

class ColMeta {
public:
  std::string tab_name; // 字段所属表名称
  std::string name;     // 字段名称
  TypeId type;          // 字段类型
  int len;              // 字段长度
  int offset;           // 字段位于记录中的偏移量
  [[maybe_unused]]
  bool index; /** unused */
  ColMeta() = default;
  ColMeta(std::string tab_name, std::string name, TypeId type, int len)
      : tab_name(std::move(tab_name)), name(std::move(name)), type(type),
        len(len) {}
  ColMeta(std::string tab_name, std::string name, ColType col_meta, int len)
      : tab_name(std::move(tab_name)), name(std::move(name)), len(len) {

    switch (col_meta) {
    case ColType::TYPE_FLOAT: {
      type = TypeId::DECIMAL;
      break;
    }
    case ColType::TYPE_STRING: {
      type = TypeId::VARCHAR;
      break;
    }
    case ColType::TYPE_INT: {
      type = TypeId::SMALLINT;
      break;
    }
    case ColType::TYPE_TIMESTAMP: {
      type = TypeId::TIMESTAMP;
    }
    default: {
      type = TypeId::INVALID;
    }
    }
  }
  ColMeta(std::string tab_name, std::string name, TypeId type, int len,
          int offset, bool index)
      : tab_name(std::move(tab_name)), name(std::move(name)), type(type),
        len(len), offset(offset), index(index) {}

  // inline ColMeta(const Column &column)
  //     : type(column.get_type()), len(column.get_len()),
  //       offset(column.get_offset()) {
  //   name = column.get_name();
  // }
  friend std::ostream &operator<<(std::ostream &os, const ColMeta &col) {
    // ColMeta中有各个基本类型的变量，然后调用重载的这些变量的操作符<<（具体实现逻辑在defs.h）
    return os << col.tab_name << ' ' << col.name << ' ' << col.type << ' '
              << col.len << ' ' << col.offset << ' ' << col.index;
  }

  friend std::istream &operator>>(std::istream &is, ColMeta &col) {
    return is >> col.tab_name >> col.name >> col.type >> col.len >>
           col.offset >> col.index;
  }
  std::string ToString() const {
    return fmt::format("name: {}, type: {}, len:{}", name, typeid2str(type),
                       len);
  }
};
template <typename T>
struct fmt::formatter<
    T, std::enable_if_t<std::is_base_of<ColMeta, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const ColMeta &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};
template <typename T>
struct fmt::formatter<
    std::unique_ptr<T>,
    std::enable_if_t<std::is_base_of<ColMeta, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormCtx>
  auto format(const std::unique_ptr<ColMeta> &x, FormCtx &ctx) {
    return fmt::formatter<std::string>::format(x->ToString(), ctx);
  }
};
