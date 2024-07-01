#include "binder/statement/drop_statement.h"
#include "format.h"
#include <string>
#include <unordered_map>
static std::unordered_map<DropType, std::string> p{
    {DropType::INVALID, "INVALID"},
    {DropType::INDEX, "INVALID"},
    {DropType::DB, "DB"},
    {DropType::TABLE, "TABLE"},
};
static std::string drop_type(DropType type) { return p.at(type); }
auto DropStatement::ToString() const -> std::string {
  return fmt::format("BoundDrop {{\n table={}\n, drop type: {} }}", drop_from,
                     drop_type(this->type));
}
