include(ExternalProject)

# libbpf
ExternalProject_Add(
    libbpf-proj
    PREFIX libbpf
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
add_library(libbpf INTERFACE)
set_target_properties(libbpf
    PROPERTIES
        SOURCE_DIR ${SOURCE_DIR}
        INSTALL_DIR ${INSTALL_DIR}
        INCLUDE_DIRS ${INSTALL_DIR}
        LIBRARIES ${INSTALL_DIR}/libbpf.a)
target_include_directories(libbpf INTERFACE ${INSTALL_DIR})
target_link_libraries(libbpf INTERFACE ${INSTALL_DIR}/libbpf.a)

# bpftool
ExternalProject_Add(
    bpftool-proj
    PREFIX bpftool
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
add_executable(bpftool IMPORTED GLOBAL)
set_target_properties(bpftool
    PROPERTIES
        IMPORTED_LOCATION ${INSTALL_DIR}bootstrap/bpftool)

