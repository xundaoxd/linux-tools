#include <iostream>

#include <gflags/gflags.h>

#include "common.h"

int main(int argc, char *argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  std::cout << "mperf" << std::endl;
  return 0;
}
