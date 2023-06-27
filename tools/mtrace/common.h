#pragma once
#include <linux/perf_event.h>
#include <linux/types.h>
#include <sys/ptrace.h>

#include <iostream>

template <typename T> void die(T &&msg) {
  std::cerr << msg << ", errno: " << errno << std::endl;
  exit(-1);
}

struct ptrace_syscall_info {
  __u8 op;                   /* Type of system call stop */
  __u32 arch;                /* AUDIT_ARCH_* value; see seccomp(2) */
  __u64 instruction_pointer; /* CPU instruction pointer */
  __u64 stack_pointer;       /* CPU stack pointer */
  union {
    struct {         /* op == PTRACE_SYSCALL_INFO_ENTRY */
      __u64 nr;      /* System call number */
      __u64 args[6]; /* System call arguments */
    } entry;
    struct {         /* op == PTRACE_SYSCALL_INFO_EXIT */
      __s64 rval;    /* System call return value */
      __u8 is_error; /* System call error flag;
                        Boolean: does rval contain
                        an error value (-ERRCODE) or
                        a nonerror return value? */
    } exit;
    struct {          /* op == PTRACE_SYSCALL_INFO_SECCOMP */
      __u64 nr;       /* System call number */
      __u64 args[6];  /* System call arguments */
      __u32 ret_data; /* SECCOMP_RET_DATA portion
                         of SECCOMP_RET_TRACE
                         return value */
    } seccomp;
  };
};

static const char *syscall_tbl[] = {"read",
                                    "write",
                                    "open",
                                    "close",
                                    "stat",
                                    "fstat",
                                    "lstat",
                                    "poll",
                                    "lseek",
                                    "mmap",
                                    "mprotect",
                                    "munmap",
                                    "brk",
                                    "rt_sigaction",
                                    "rt_sigprocmask",
                                    "rt_sigreturn",
                                    "ioctl",
                                    "pread64",
                                    "pwrite64",
                                    "readv",
                                    "writev",
                                    "access",
                                    "pipe",
                                    "select",
                                    "sched_yield",
                                    "mremap",
                                    "msync",
                                    "mincore",
                                    "madvise",
                                    "shmget",
                                    "shmat",
                                    "shmctl",
                                    "dup",
                                    "dup2",
                                    "pause",
                                    "nanosleep",
                                    "getitimer",
                                    "alarm",
                                    "setitimer",
                                    "getpid",
                                    "sendfile",
                                    "socket",
                                    "connect",
                                    "accept",
                                    "sendto",
                                    "recvfrom",
                                    "sendmsg",
                                    "recvmsg",
                                    "shutdown",
                                    "bind",
                                    "listen",
                                    "getsockname",
                                    "getpeername",
                                    "socketpair",
                                    "setsockopt",
                                    "getsockopt",
                                    "clone",
                                    "fork",
                                    "vfork",
                                    "execve",
                                    "exit",
                                    "wait4",
                                    "kill",
                                    "uname",
                                    "semget",
                                    "semop",
                                    "semctl",
                                    "shmdt",
                                    "msgget",
                                    "msgsnd",
                                    "msgrcv",
                                    "msgctl",
                                    "fcntl",
                                    "flock",
                                    "fsync",
                                    "fdatasync",
                                    "truncate",
                                    "ftruncate",
                                    "getdents",
                                    "getcwd",
                                    "chdir",
                                    "fchdir",
                                    "rename",
                                    "mkdir",
                                    "rmdir",
                                    "creat",
                                    "link",
                                    "unlink",
                                    "symlink",
                                    "readlink",
                                    "chmod",
                                    "fchmod",
                                    "chown",
                                    "fchown",
                                    "lchown",
                                    "umask",
                                    "gettimeofday",
                                    "getrlimit",
                                    "getrusage",
                                    "sysinfo",
                                    "times",
                                    "ptrace",
                                    "getuid",
                                    "syslog",
                                    "getgid",
                                    "setuid",
                                    "setgid",
                                    "geteuid",
                                    "getegid",
                                    "setpgid",
                                    "getppid",
                                    "getpgrp",
                                    "setsid",
                                    "setreuid",
                                    "setregid",
                                    "getgroups",
                                    "setgroups",
                                    "setresuid",
                                    "getresuid",
                                    "setresgid",
                                    "getresgid",
                                    "getpgid",
                                    "setfsuid",
                                    "setfsgid",
                                    "getsid",
                                    "capget",
                                    "capset",
                                    "rt_sigpending",
                                    "rt_sigtimedwait",
                                    "rt_sigqueueinfo",
                                    "rt_sigsuspend",
                                    "sigaltstack",
                                    "utime",
                                    "mknod",
                                    "uselib",
                                    "personality",
                                    "ustat",
                                    "statfs",
                                    "fstatfs",
                                    "sysfs",
                                    "getpriority",
                                    "setpriority",
                                    "sched_setparam",
                                    "sched_getparam",
                                    "sched_setscheduler",
                                    "sched_getscheduler",
                                    "sched_get_priority_max",
                                    "sched_get_priority_min",
                                    "sched_rr_get_interval",
                                    "mlock",
                                    "munlock",
                                    "mlockall",
                                    "munlockall",
                                    "vhangup",
                                    "modify_ldt",
                                    "pivot_root",
                                    "_sysctl",
                                    "prctl",
                                    "arch_prctl",
                                    "adjtimex",
                                    "setrlimit",
                                    "chroot",
                                    "sync",
                                    "acct",
                                    "settimeofday",
                                    "mount",
                                    "umount2",
                                    "swapon",
                                    "swapoff",
                                    "reboot",
                                    "sethostname",
                                    "setdomainname",
                                    "iopl",
                                    "ioperm",
                                    "create_module",
                                    "init_module",
                                    "delete_module",
                                    "get_kernel_syms",
                                    "query_module",
                                    "quotactl",
                                    "nfsservctl",
                                    "getpmsg",
                                    "putpmsg",
                                    "afs_syscall",
                                    "tuxcall",
                                    "security",
                                    "gettid",
                                    "readahead",
                                    "setxattr",
                                    "lsetxattr",
                                    "fsetxattr",
                                    "getxattr",
                                    "lgetxattr",
                                    "fgetxattr",
                                    "listxattr",
                                    "llistxattr",
                                    "flistxattr",
                                    "removexattr",
                                    "lremovexattr",
                                    "fremovexattr",
                                    "tkill",
                                    "time",
                                    "futex",
                                    "sched_setaffinity",
                                    "sched_getaffinity",
                                    "set_thread_area",
                                    "io_setup",
                                    "io_destroy",
                                    "io_getevents",
                                    "io_submit",
                                    "io_cancel",
                                    "get_thread_area",
                                    "lookup_dcookie",
                                    "epoll_create",
                                    "epoll_ctl_old",
                                    "epoll_wait_old",
                                    "remap_file_pages",
                                    "getdents64",
                                    "set_tid_address",
                                    "restart_syscall",
                                    "semtimedop",
                                    "fadvise64",
                                    "timer_create",
                                    "timer_settime",
                                    "timer_gettime",
                                    "timer_getoverrun",
                                    "timer_delete",
                                    "clock_settime",
                                    "clock_gettime",
                                    "clock_getres",
                                    "clock_nanosleep",
                                    "exit_group",
                                    "epoll_wait",
                                    "epoll_ctl",
                                    "tgkill",
                                    "utimes",
                                    "vserver",
                                    "mbind",
                                    "set_mempolicy",
                                    "get_mempolicy",
                                    "mq_open",
                                    "mq_unlink",
                                    "mq_timedsend",
                                    "mq_timedreceive",
                                    "mq_notify",
                                    "mq_getsetattr",
                                    "kexec_load",
                                    "waitid",
                                    "add_key",
                                    "request_key",
                                    "keyctl",
                                    "ioprio_set",
                                    "ioprio_get",
                                    "inotify_init",
                                    "inotify_add_watch",
                                    "inotify_rm_watch",
                                    "migrate_pages",
                                    "openat",
                                    "mkdirat",
                                    "mknodat",
                                    "fchownat",
                                    "futimesat",
                                    "newfstatat",
                                    "unlinkat",
                                    "renameat",
                                    "linkat",
                                    "symlinkat",
                                    "readlinkat",
                                    "fchmodat",
                                    "faccessat",
                                    "pselect6",
                                    "ppoll",
                                    "unshare",
                                    "set_robust_list",
                                    "get_robust_list",
                                    "splice",
                                    "tee",
                                    "sync_file_range",
                                    "vmsplice",
                                    "move_pages",
                                    "utimensat",
                                    "epoll_pwait",
                                    "signalfd",
                                    "timerfd_create",
                                    "eventfd",
                                    "fallocate",
                                    "timerfd_settime",
                                    "timerfd_gettime",
                                    "accept4",
                                    "signalfd4",
                                    "eventfd2",
                                    "epoll_create1",
                                    "dup3",
                                    "pipe2",
                                    "inotify_init1",
                                    "preadv",
                                    "pwritev",
                                    "rt_tgsigqueueinfo",
                                    "perf_event_open",
                                    "recvmmsg",
                                    "fanotify_init",
                                    "fanotify_mark",
                                    "prlimit64",
                                    "name_to_handle_at",
                                    "open_by_handle_at",
                                    "clock_adjtime",
                                    "syncfs",
                                    "sendmmsg",
                                    "setns",
                                    "getcpu",
                                    "process_vm_readv",
                                    "process_vm_writev",
                                    "kcmp",
                                    "finit_module"};

static void peek_data(void *dst, int size, pid_t pid, __u64 addr) {
  for (int i = 0; i < size; i++) {
    ((long *)(dst))[i] =
        ptrace(PTRACE_PEEKDATA, pid, addr + i * sizeof(long), NULL);
  }
}

static void perf_event_open_handler(pid_t pid,
                                    const struct ptrace_syscall_info &info) {
  std::cout << "perf_event_open\n";
  struct perf_event_attr attr;
  peek_data(&attr, sizeof(attr), pid, info.entry.args[0]);

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

inline void do_syscall(pid_t pid, const struct ptrace_syscall_info &info) {
  if (info.op == PTRACE_SYSCALL_INFO_ENTRY) {
    if (info.entry.nr < sizeof(syscall_tbl) / sizeof(syscall_tbl[0])) {
      switch (info.entry.nr) {
      case 298:
        perf_event_open_handler(pid, info);
        break;
      default:
        std::cout << info.entry.nr << ":" << syscall_tbl[info.entry.nr]
                  << std::endl;
        break;
      }
    } else {
      std::cerr << "invalid syscall, nr: " << info.entry.nr << std::endl;
    }
  }
}
