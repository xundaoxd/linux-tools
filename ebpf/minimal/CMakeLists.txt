ebpf_skel(minimal_skel minimal.bpf.c)

add_executable(minimal minimal.cpp)
target_link_libraries(minimal minimal_skel)
