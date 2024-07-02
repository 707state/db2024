/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL
v2. You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "common/exception.h"
#include "fmt/format.h"
#include "record/rm_defs.h"
#include "type/type_id.h"
#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
class Page;
struct TabCol {
  std::string tab_name;
  std::string col_name;

  friend bool operator<(const TabCol &x, const TabCol &y) {
    return std::make_pair(x.tab_name, x.col_name) <
           std::make_pair(y.tab_name, y.col_name);
  }
};

struct sValue {
  ColType type; // type of value
  union {
    int int_val;      // int value
    double float_val; // float value
  };
  std::string str_val; // string value
  explicit sValue(ColType type, int value) {
    if (type == ColType::TYPE_INT) {
      int_val = value;
    }
    if (type == ColType::TYPE_FLOAT) {
      float_val = static_cast<double>(value);
    }
    if (type == ColType::TYPE_STRING) {
      throw Exception("Cannot bind string in this value");
    }
  }
  explicit sValue(ColType type, double value) {
    if (type == ColType::TYPE_FLOAT) {
      float_val = value;
    }
    throw Exception("Value not right");
  }
  sValue(ColType type, std::string val) {
    if (type == ColType::TYPE_STRING) {
      str_val = std::move(val);
    }
    throw Exception("Value not right");
  }
  sValue(TypeId id, int val) : type(typeid_to_coltype(id)) { int_val = val; }
  sValue(TypeId id, double val) {
    type = typeid_to_coltype(id);
    float_val = val;
  }
  sValue(TypeId id, std::string val) {
    type = typeid_to_coltype(id);
    str_val = std::move(val);
  }
  std::shared_ptr<RmRecord> raw; // raw record buffer

  void set_int(int int_val_) {
    type = ColType::TYPE_INT;
    int_val = int_val_;
  }
  std::string ToString() {
    return fmt::format("int_val={}, float_val={}, str_val={}", int_val,
                       float_val, str_val);
  }
  std::string ToString() const {
    return fmt::format("int_val={}, float_val={}, str_val={}", int_val,
                       float_val, str_val);
  }
  void set_float(float float_val_) {
    type = ColType::TYPE_FLOAT;
    float_val = float_val_;
  }

  void set_str(std::string str_val_) {
    type = ColType::TYPE_STRING;
    str_val = std::move(str_val_);
  }

  void init_raw(int len) {
    assert(raw == nullptr);
    raw = std::make_shared<RmRecord>(len);
    if (type == ColType::TYPE_INT) {
      assert(len == sizeof(int));
      *(int *)(raw->data) = int_val;
    } else if (type == ColType::TYPE_FLOAT) {
      assert(len == sizeof(float));
      *(float *)(raw->data) = float_val;
    } else if (type == ColType::TYPE_STRING) {
      if (len < (int)str_val.size()) {
        throw Exception("String overflow");
      }
      memset(raw->data, 0, len);
      memcpy(raw->data, str_val.c_str(), str_val.size());
    }
  }
};

enum CompOp { OP_EQ, OP_NE, OP_LT, OP_GT, OP_LE, OP_GE };

struct Condition {
  TabCol lhs_col;  // left-hand side column
  CompOp op;       // comparison operator
  bool is_rhs_val; // true if right-hand side is a value (not a column)
  TabCol rhs_col;  // right-hand side column
  sValue rhs_val;  // right-hand side value
};

struct SetClause {
  TabCol lhs;
  sValue rhs;
};
