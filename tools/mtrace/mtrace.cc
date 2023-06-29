#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

void doChild(int argc, char *argv[]) {
  ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  execve(argv[0], argv, NULL);
  die("child execve failed");
}

void doParent(pid_t pid) {
  int status;
  wait(&status);
  if (WIFEXITED(status)) {
    return;
  }
  ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_TRACESYSGOOD);
  while (1) {
    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    wait(&status);
    if (WIFEXITED(status)) {
      break;
    }

    struct ptrace_syscall_info info;
    ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(info), &info);
    if (info.op) {
      do_syscall(pid, info);
    }
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
