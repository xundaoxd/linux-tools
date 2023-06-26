#include <algorithm>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <vector>

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
  ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
  while (1) {
    wait(&status);
    if (WIFEXITED(status)) {
      break;
    }
    std::cout << "new syscall" << std::endl;
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
