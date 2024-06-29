#include "column.h"
#include "defs.h"
#include <sstream>
#include <string>
auto Column::to_string(bool simplified) const -> std::string {
  std::ostringstream os;

  if (simplified) {
    os << col_name_ << ":" << coltype2str(type_id);
    if (type_id == ColType::TYPE_STRING) {
      os << "(" << col_len_ << ")";
    }
    return (os.str());
  }
  os << "Column[" << col_name_ << ", " << coltype2str(type_id) << ", "
     << "Offset:" << col_offset_ << ", " << "Length:" << col_len_ << "]";
  return (os.str());
}

