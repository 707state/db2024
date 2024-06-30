#pragma once

#include <string>
#include <utility>

#include "binder/bound_expression.h"
#include "common/common.h"

class BoundExpression;

/**
 * A bound constant, e.g., `1`.
 */
class BoundConstant : public BoundExpression {
public:
  explicit BoundConstant(sValue val)
      : BoundExpression(ExpressionType::CONSTANT), val_(std::move(val)) {}

  auto ToString() const -> std::string override { return val_.ToString(); }

  auto HasAggregation() const -> bool override { return false; }

  /** The constant being bound. */
  sValue val_;
};
