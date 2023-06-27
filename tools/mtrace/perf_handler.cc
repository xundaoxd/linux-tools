#include "perf_handler.h"

#include <linux/perf_event.h>
#include <linux/types.h>

#include <algorithm>
#include <iostream>

#include "common.h"

void perf_event_open_handler(pid_t pid,
                             const struct ptrace_syscall_info &info) {
  std::cout << "perf_event_open\n";
  long size =
      ptrace(PTRACE_PEEKDATA, pid, info.entry.args[0] + sizeof(__u32), NULL) &
      0xffffffff;
  struct perf_event_attr attr;
  peek_data(&attr, size > sizeof(attr) ? sizeof(attr) : size, pid,
            info.entry.args[0]);

  std::cout << "    type: 0x" << std::hex << attr.type << std::endl;
  std::cout << "    size: 0x" << std::hex << attr.size << std::endl;
  std::cout << "    config: 0x" << std::hex << attr.config << std::endl;
  std::cout << "    sample_period: 0x" << std::hex << attr.sample_period
            << std::endl;
  std::cout << "    sample_freq: 0x" << std::hex << attr.sample_freq
            << std::endl;
  std::cout << "    sample_type: 0x" << std::hex << attr.sample_type
            << std::endl;
  std::cout << "    read_format: 0x" << std::hex << attr.read_format
            << std::endl;

  std::cout << "    mmap: 0x" << std::hex << attr.mmap << std::endl;
  std::cout << "    freq: 0x" << std::hex << attr.freq << std::endl;
}
