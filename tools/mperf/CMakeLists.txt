find_package(gflags REQUIRED)

add_executable(mperf mperf.cc)
target_link_libraries(mperf gflags)

