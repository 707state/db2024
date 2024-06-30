#include "base.h"
#include "ranges.h"
#include <cstdio>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
TEST(FMT_Test, SimpleTest) {
  std::vector<std::string> str;
  for (int i = 0; i < 100; i++) {
    str.push_back(fmt::to_string(i));
  }
  std::cout << fmt::format("{}", fmt::join(str, ", "));
}
