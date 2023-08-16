#include "minimal.skel.h"

#include <bpf/libbpf.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int err;

  minimal *skel = minimal__open();
  if (!skel) {
    fprintf(stderr, "Failed to open BPF skeleton\n");
    return 1;
  }

  /* ensure BPF program only handles write() syscalls from our process */
  skel->bss->my_pid = getpid();

  /* Load & verify BPF programs */
  err = minimal__load(skel);
  if (err) {
    fprintf(stderr, "Failed to load and verify BPF skeleton\n");
    goto cleanup;
  }

  /* Attach tracepoint handler */
  err = minimal__attach(skel);
  if (err) {
    fprintf(stderr, "Failed to attach BPF skeleton\n");
    goto cleanup;
  }

  printf("Successfully started! Please run `sudo cat "
         "/sys/kernel/debug/tracing/trace_pipe` "
         "to see output of the BPF programs.\n");

  for (;;) {
    /* trigger our BPF program */
    fprintf(stderr, ".");
    sleep(1);
  }

cleanup:
  minimal__destroy(skel);
  return -err;
}
