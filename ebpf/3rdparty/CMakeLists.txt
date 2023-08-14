include(ExternalProject)

# libbpf
ExternalProject_Add(
    libbpf-proj
    GIT_REPOSITORY      https://github.com/libbpf/libbpf.git
    GIT_TAG             f8cd00f61302ca4d8645e007acf7f546dc323b33
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       make
        BUILD_STATIC_ONLY=1
        DESTDIR=<INSTALL_DIR>
        INCLUDEDIR=
        LIBDIR=
        UAPIDIR=
        -C <SOURCE_DIR>/src
        install install_uapi_headers
    INSTALL_COMMAND     ""
    STEP_TARGETS        build
)
ExternalProject_Get_property(libbpf-proj INSTALL_DIR SOURCE_DIR)
set(libbpf_INSTALL_DIR ${INSTALL_DIR})
set(libbpf_SOURCE_DIR ${SOURCE_DIR})
set(libbpf_INCLUDE_DIRS ${libbpf_INSTALL_DIR} ${libbpf_SOURCE_DIR}/include/uapi)
set(libbpf_LIBRARIES ${libbpf_INSTALL_DIR}/libbpf.a)

add_library(libbpf INTERFACE)
target_include_directories(libbpf INTERFACE ${libbpf_INCLUDE_DIRS})
target_link_libraries(libbpf INTERFACE ${libbpf_LIBRARIES})

# bpftool
ExternalProject_Add(
    bpftool-proj
    GIT_REPOSITORY      https://github.com/libbpf/bpftool.git
    GIT_TAG             259f40fd5bde67025beb1371d269c91bbe5d42ed
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       make
        bootstrap
        -C <SOURCE_DIR>/src
        OUTPUT=<INSTALL_DIR>
    INSTALL_COMMAND     ""
    STEP_TARGETS        build
)
ExternalProject_Get_property(bpftool-proj INSTALL_DIR)
set(bpftool_ROOT_DIR ${INSTALL_DIR})
set(bpftool_EXECUTABLE ${bpftool_ROOT_DIR}bootstrap/bpftool)

add_executable(bpftool IMPORTED GLOBAL)
set_property(TARGET bpftool PROPERTY
             IMPORTED_LOCATION ${bpftool_EXECUTABLE})

# blazesym
# find_program(CARGO_EXISTS cargo)
# ExternalProject_Add(
#     blazesym
#     GIT_REPOSITORY      https://github.com/libbpf/blazesym.git
#     GIT_TAG             923461112819076a59d74c71320d143f2ed099fd
#     BUILD_IN_SOURCE     TRUE
#     CONFIGURE_COMMAND   ""
#     BUILD_COMMAND        cargo build --release
#     INSTALL_COMMAND     ""
#     STEP_TARGETS        build
# )
# ExternalProject_Get_property(blazesym SOURCE_DIR)

