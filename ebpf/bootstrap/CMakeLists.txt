ebpf_skel(bootstrap_skel bootstrap.bpf.c)

add_executable(bootstrap bootstrap.cpp)
target_link_libraries(bootstrap bootstrap_skel)

