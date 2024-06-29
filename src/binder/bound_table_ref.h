#pragma once

#include <memory>
#include <string>

#include "fmt/format.h"

#include "common/macros.h"

/**
 * Table reference types.
 */
enum class TableReferenceType : uint8_t {
  INVALID = 0,         /**< Invalid table reference type. */
  BASE_TABLE = 1,      /**< Base table reference. */
  JOIN = 3,            /**< Output of join. */
  CROSS_PRODUCT = 4,   /**< Output of cartesian product. */
  EXPRESSION_LIST = 5, /**< Values clause. */
  SUBQUERY = 6,        /**< Subquery. */
  CTE = 7,             /**< CTE. */
  EMPTY = 8            /**< Placeholder for empty FROM. */
};

/**
 * A bound table reference.
 */
class BoundTableRef {
public:
  explicit BoundTableRef(TableReferenceType type) : type_(type) {}
  BoundTableRef() = default;
  virtual ~BoundTableRef() = default;

  virtual auto ToString() const -> std::string {
    switch (type_) {
    case TableReferenceType::INVALID:
      return "";
    case TableReferenceType::EMPTY:
      return "<empty>";
    default:
      // For other types of table reference, `ToString` should be derived in
      // child classes.
      UNREACHABLE("entered unreachable code");
    }
  }

  auto IsInvalid() const -> bool {
    return type_ == TableReferenceType::INVALID;
  }

  /** The type of table reference. */
  TableReferenceType type_{TableReferenceType::INVALID};
};

template <typename T>
struct fmt::formatter<
    T, std::enable_if_t<std::is_base_of<BoundTableRef, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx> auto format(const T &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<
    std::unique_ptr<T>,
    std::enable_if_t<std::is_base_of<BoundTableRef, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<T> &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x->ToString(), ctx);
  }
};

template <> struct fmt::formatter<TableReferenceType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(TableReferenceType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
    case TableReferenceType::INVALID:
      name = "Invalid";
      break;
    case TableReferenceType::BASE_TABLE:
      name = "BaseTable";
      break;
    case TableReferenceType::JOIN:
      name = "Join";
      break;
    case TableReferenceType::CROSS_PRODUCT:
      name = "CrossProduct";
      break;
    case TableReferenceType::EMPTY:
      name = "Empty";
      break;
    case TableReferenceType::EXPRESSION_LIST:
      name = "ExpressionList";
      break;
    case TableReferenceType::SUBQUERY:
      name = "Subquery";
      break;
    case TableReferenceType::CTE:
      name = "CTE";
      break;
    default:
      name = "Unknown";
      break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
