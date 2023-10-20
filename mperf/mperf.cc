#include <string.h>
#include <sys/ioctl.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "common.h"

namespace {

class PerfEvent {
  int fd_;

public:
  PerfEvent() {
    if (good()) {
      close(fd());
    }
  }
  PerfEvent(const perf_event_attr &pe, pid_t pid, int cpu, int group_fd = -1,
            unsigned long flags = PERF_FLAG_FD_CLOEXEC) {
    fd_ = perf_event_open(&pe, pid, cpu, group_fd, flags);
  }

  int fd() const { return fd_; }
  bool good() const { return fd() != -1; }

  void Reset() { ioctl(fd(), PERF_EVENT_IOC_RESET, 0); }
  void Enable() { ioctl(fd(), PERF_EVENT_IOC_ENABLE, 0); }
  void Disable() { ioctl(fd(), PERF_EVENT_IOC_DISABLE, 0); }
};

} // namespace

int main(int argc, char *argv[]) {
  struct perf_event_attr pe;
  memset(&pe, 0, sizeof(struct perf_event_attr));
  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(struct perf_event_attr);
  pe.config = PERF_COUNT_HW_INSTRUCTIONS;
  pe.disabled = 1;
  pe.exclude_kernel = 1;
  pe.exclude_hv = 1;

  std::vector<PerfEvent> pevents;
  for (int i = 0; i < 16; i++) {
    pevents.emplace_back(pe, -1, i);
    assert(pevents[i].good());
  }

  for (auto &&item : pevents) {
    item.Reset();
    item.Enable();
  }

  while (1) {
    struct {
      uint64_t value;
    } result;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    for (auto &&item : pevents) {
      ssize_t bytes = read(item.fd(), &result, sizeof(result));
      std::cout << " " << result.value << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
