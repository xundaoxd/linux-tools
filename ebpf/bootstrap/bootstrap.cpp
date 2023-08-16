#include <signal.h>
#include <stdio.h>
#include <time.h>

#include <bpf/libbpf.h>

#include "bootstrap.h"
#include "bootstrap.skel.h"

static int handle_event(void *ctx, void *data, size_t data_sz) {
  const struct event *e = (const struct event *)data;
  struct tm *tm;
  char ts[32];
  time_t t;

  time(&t);
  tm = localtime(&t);
  strftime(ts, sizeof(ts), "%H:%M:%S", tm);

  printf("%-8s %-16s %-7d %-7d %s\n", ts, e->comm, e->pid, e->ppid,
         e->filename);
  return 0;
}

int status = 0;
void signal_handler(int) { status = 1; }

int main(int argc, char *argv[]) {
  signal(SIGINT, signal_handler);

  int err;
  struct ring_buffer *rb;

  struct bootstrap_bpf *skel = bootstrap_bpf__open();
  if (!skel) {
    fprintf(stderr, "Failed to open and load BPF skeleton\n");
    return 1;
  }
  err = bootstrap_bpf__load(skel);
  if (err) {
    fprintf(stderr, "Failed to load and verify BPF skeleton\n");
    goto cleanup;
  }
  err = bootstrap_bpf__attach(skel);
  if (err) {
    fprintf(stderr, "Failed to attach BPF skeleton\n");
    goto cleanup;
  }

  rb = ring_buffer__new(bpf_map__fd(skel->maps.rb), handle_event, NULL, NULL);
  if (!rb) {
    err = -1;
    fprintf(stderr, "Failed to create ring buffer\n");
    goto cleanup;
  }

  while (!status) {
    err = ring_buffer__poll(rb, 100 /* timeout, ms */);
    /* Ctrl-C will cause -EINTR */
    if (err == -EINTR) {
      err = 0;
      break;
    }
    if (err < 0) {
      printf("Error polling perf buffer: %d\n", err);
      break;
    }
  }

cleanup:
  ring_buffer__free(rb);
  bootstrap_bpf__destroy(skel);

  return 0;
}
