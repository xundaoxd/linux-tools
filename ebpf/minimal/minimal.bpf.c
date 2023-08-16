#include <bpf/bpf_helpers.h>

SEC("tp/syscalls/sys_enter_write")
int handle_tp(void *ctx) {
  int pid = bpf_get_current_pid_tgid() >> 32;

  char comm[TASK_COMM_LEN];
  bpf_get_current_comm(&comm, sizeof(comm));

  bpf_printk("sys_enter_write, comm: %s, pid: %d", comm, pid);
  return 0;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
