#pragma once
#include "defs.h"
#include "system/column.h"
#include <string>
#include <utility>
namespace rmdb {
class Column;

class ColMeta {
public:
  std::string tab_name; // 字段所属表名称
  std::string name;     // 字段名称
  ColType type;         // 字段类型
  int len;              // 字段长度
  int offset;           // 字段位于记录中的偏移量
  [[maybe_unused]]
  bool index; /** unused */
  ColMeta() = default;
  ColMeta(std::string tab_name, std::string name, ColType type, int len)
      : tab_name(std::move(tab_name)), name(std::move(name)), type(type),
        len(len) {}
  ColMeta(std::string tab_name, std::string name, ColType type, int len,
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
};
} // namespace rmdb
