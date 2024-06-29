#include <atomic>
#include <chrono>
using std::atomic;
using std::chrono::duration;
using std::chrono::milliseconds;

atomic<bool> enable_logging(false);

duration<int64_t> log_timeout = std::chrono::seconds(1);

milliseconds cycle_detection_interval = std::chrono::milliseconds(50);
