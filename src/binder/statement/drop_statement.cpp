#include "binder/statement/drop_statement.h"
#include "format.h"
#include <string>
auto DropStatement::ToString() const -> std::string {
  return fmt::format("BoundDrop {{\n table={}\n }}", tab_name_);
}
