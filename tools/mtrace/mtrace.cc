#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "common.h"

template <typename T> void die(T &&msg) {
  std::cout << msg << ", errno: " << errno << std::endl;
  exit(-1);
}

void doChild(int argc, char *argv[]) {
  std::vector<char *> args(argc, NULL);
  std::copy_n(argv + 1, argc - 1, args.begin());

  ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  execve(argv[0], args.data(), NULL);
  die("child execve failed");
}

void doParent(pid_t pid) {
  int status;
  wait(&status);
  if (WIFEXITED(status)) {
    return;
  }
  ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_TRACESYSGOOD);
  ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
  while (1) {
    wait(&status);
    if (WIFEXITED(status)) {
      break;
    }

    struct ptrace_syscall_info info;
    ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(info), &info);
    if (info.op == PTRACE_SYSCALL_INFO_ENTRY) {
      if (info.entry.nr < sizeof(syscall_tbl) / sizeof(syscall_tbl[0])) {
        std::cout << info.entry.nr << ":" << syscall_tbl[info.entry.nr]
                  << std::endl;
      } else {
        std::cout << "undefined syscall, nr: " << info.entry.nr << std::endl;
      }
    }

    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
  }
}

int main(int argc, char *argv[]) {
  argc--;
  argv++;
  if (!argc) {
    die("target program args invalid");
  }
  pid_t pid = fork();
  switch (pid) {
  case -1:
    die("fork failed");
    break;
  case 0:
    doChild(argc, argv);
    break;
  default:
    doParent(pid);
    break;
  }

  return 0;
}
