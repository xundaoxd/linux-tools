ebpf_skel(syscall_skel syscall.bpf.c)

add_executable(syscall syscall.cpp)
target_link_libraries(syscall syscall_skel)
