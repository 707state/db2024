#define DEBUG
#include <common/err_message.h>
#include <gtest/gtest.h>
TEST(ErrMessageTest, SimpleTest) {
  LOG_WARNING("Test %d\n", 1);
  auto file = __FILE__;
  auto line = __LINE__;
  LOG_INFO("Test log_info. %s %s", "1", "test");
  LOG_ERROR("Test log_error. %d %d", 2, 4);
  LOG_ERROR("Test log_error, %s",
            strrc(RC_VALUES::RC_LRU_REPLACER_UNPIN_ERROR).c_str());
}
