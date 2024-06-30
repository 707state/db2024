#pragma once
// Every possible SQL type ID
#include <cstddef>
#include <map>
#include <string>
enum class ColType {
  INVALID,
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_STRING,
  TYPE_TIMESTAMP
};

enum class TypeId {
  INVALID = 0,
  BOOLEAN,
  TINYINT,
  SMALLINT,
  INTEGER,
  BIGINT,
  DECIMAL,
  VARCHAR,
  TIMESTAMP
};
static ColType typeid_to_coltype(TypeId id) {
  switch (id) {
  case TypeId::BIGINT:
  case TypeId::TINYINT:
  case TypeId::SMALLINT:
  case TypeId::INTEGER: {
    return ColType::TYPE_INT;
  }
  case TypeId::DECIMAL: {
    return ColType::TYPE_FLOAT;
  }
  case TypeId::VARCHAR: {
    return ColType::TYPE_STRING;
  }
  case TypeId::TIMESTAMP: {
    return ColType::TYPE_TIMESTAMP;
  }
  default: {
    return ColType::INVALID;
  }
  }
}
static TypeId coltype_to_typeid(ColType col_type) {
  switch (col_type) {
  case ColType::TYPE_INT: {
    return TypeId::INTEGER;
  }
  case ColType::TYPE_FLOAT: {
    return TypeId::DECIMAL;
  }
  case ColType::TYPE_STRING: {
    return TypeId::VARCHAR;
  }
  case ColType::TYPE_TIMESTAMP: {
    return TypeId::TIMESTAMP;
  }
  default: {
    return TypeId::INVALID;
  }
  }
}
inline size_t coltype2len(ColType type, size_t len = 4) {
  switch (type) {
  case ColType::TYPE_INT: {
    return 4;
  }
  case ColType::TYPE_FLOAT: {
    return 8;
  }
  case ColType::TYPE_TIMESTAMP: {
    return 8;
  }
  case ColType::TYPE_STRING: {
    return len;
  }
  default:
    return 0;
    break;
  }
}

inline std::string coltype2str(ColType type) {
  std::map<ColType, std::string> m = {{ColType::TYPE_INT, "INT"},
                                      {ColType::TYPE_FLOAT, "FLOAT"},
                                      {ColType::TYPE_STRING, "STRING"}};
  return m.at(type);
}
inline std::string typeid2str(TypeId id) {
  std::map<TypeId, std::string> m = {
      {TypeId::BIGINT, "BIGINT"},   {TypeId::SMALLINT, "SMALLINT"},
      {TypeId::TINYINT, "TINYINT"}, {TypeId::DECIMAL, "FLOAT"},
      {TypeId::VARCHAR, "STRING"},  {TypeId::TIMESTAMP, "TIMESTAMP"}};
  return m.at(id);
}
