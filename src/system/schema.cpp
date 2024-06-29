#include "system/schema.h"
#include "defs.h"
#include "system/col_meta.h"
#include "system/column.h"
#include "system/sm_meta.h"
#include <cstddef>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
namespace rmdb {
Schema::Schema(const std::vector<Column> &columns, const std::string &tab_name)
    : tab_name(tab_name) {
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

// Schema::Schema(const std::vector<Column> &columns) {
//   size_t curr_offset = 0;
//   for (auto index = 0; index < columns.size(); index++) {
//     auto column = columns[index];
//     if (!column.is_inlined()) {
//       tuple_is_inlined_ = false;
//       schema_uninlined_cols_.push_back(index);
//     }
//     column.set_offset(curr_offset);
//     if (column.is_inlined()) {
//       curr_offset += column.get_len();
//     } else {
//       // 几种数据的长度不一致。
//       curr_offset += column.get_len();
//     }
//     schema_cols_.push_back(ColMeta{this->tab_name.value(), column.get_name(),
//                                    column.get_type(),
//                                    static_cast<int>(column.get_len())});
//   }
//   schema_len_ = curr_offset;
// }
// Schema::Schema(const std::vector<ColMeta> &column_metas)
//     : tab_name(std::nullopt) {
//
//   size_t curr_offset = 0;
//   for (auto index = 0; index < column_metas.size(); index++) {
//     auto column = column_metas[index];
//     if (column.type == ColType::TYPE_STRING) {
//       tuple_is_inlined_ = false;
//       schema_uninlined_cols_.push_back(index
//
//       );
//     }
//     column.offset = curr_offset;
//     curr_offset += column.len;
//     schema_cols_.push_back(column);
//   }
//   schema_len_ = curr_offset;
// }
auto Schema::to_string(bool simplified) const -> std::string {
  std::ostringstream os;
  if (simplified) {
  }
}
} // namespace rmdb
