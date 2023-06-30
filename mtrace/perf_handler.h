#pragma once
#include <linux/types.h>
#include <sys/types.h>

void perf_event_open_handler(pid_t pid, const struct ptrace_syscall_info &info);
