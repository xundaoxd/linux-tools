#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace {

inline long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
  int ret;

  ret = syscall(SYS_perf_event_open, hw_event, pid, cpu, group_fd, flags);
  return ret;
}

} // namespace
