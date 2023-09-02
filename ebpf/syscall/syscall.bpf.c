#include <bpf/bpf_helpers.h>

SEC("ksyscall/write")
int BPF_KSYSCALL(write_entry) {
  int pid = bpf_get_current_pid_tgid() >> 32;

  char comm[TASK_COMM_LEN];
  bpf_get_current_comm(&comm, sizeof(comm));

  bpf_printk("write_entry, pid: %d, comm: %s", pid, comm);
  return 0;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
