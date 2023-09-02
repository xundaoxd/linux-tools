include(ExternalProject)

# libbpf
ExternalProject_Add(
    libbpf-proj
    PREFIX libbpf
    GIT_REPOSITORY      https://github.com/libbpf/libbpf.git
    GIT_TAG             f8cd00f61302ca4d8645e007acf7f546dc323b33
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       make
        CC=${CMAKE_C_COMPILER}
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
        BPF_INCLUDE_DIRECTORIES "${INSTALL_DIR};${SOURCE_DIR}/include/uapi"
)
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
        CC=${CMAKE_C_COMPILER}
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

macro(ebpf_skel name)
    # clang
    execute_process(
        COMMAND bash -c "clang -v -E - < /dev/null 2>&1 |
                sed -n '/<...> search starts here:/,/End of search list./{ s| \\(/.*\\)|-idirafter \\1|p }'"
        OUTPUT_VARIABLE CLANG_SYSTEM_INCLUDES_output
        ERROR_VARIABLE CLANG_SYSTEM_INCLUDES_error
        RESULT_VARIABLE CLANG_SYSTEM_INCLUDES_result
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(${CLANG_SYSTEM_INCLUDES_result} EQUAL 0)
        separate_arguments(CLANG_SYSTEM_INCLUDES UNIX_COMMAND ${CLANG_SYSTEM_INCLUDES_output})
        message(STATUS "BPF system include flags: ${CLANG_SYSTEM_INCLUDES}")
    else()
        message(FATAL_ERROR "Failed to determine BPF system includes: ${CLANG_SYSTEM_INCLUDES_error}")
    endif()

    # arch
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
        set(ARCH "x86")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
        set(ARCH "arm")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        set(ARCH "arm64")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc64le")
        set(ARCH "powerpc")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "mips")
        set(ARCH "mips")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "riscv64")
        set(ARCH "riscv")
    elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "loongarch64")
        set(ARCH "loongarch")
    endif()

    # vmlinux
    set(VMLINUX_DIR ${CMAKE_SOURCE_DIR}/vmlinux/${ARCH})

    add_library(${name} INTERFACE)
    target_link_libraries(${name} INTERFACE libbpf elf z)
    target_include_directories(${name} INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
    foreach(item ${ARGN})
        get_filename_component(item_namewe ${item} NAME_WE)
        set(BPF_SKEL_FILE ${CMAKE_CURRENT_BINARY_DIR}/${item_namewe}.skel.h)
        set(BPF_O_FILE ${CMAKE_CURRENT_BINARY_DIR}/${item_namewe}.bpf.o)
        add_custom_target(
            ${item_namewe}_h
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND clang -g -O2 -target bpf -D__TARGET_ARCH_${ARCH}
                    ${CLANG_SYSTEM_INCLUDES} -I${VMLINUX_DIR}
                    "-isystem;$<JOIN:$<TARGET_PROPERTY:libbpf,BPF_INCLUDE_DIRECTORIES>,;-isystem;>"
                    --include=vmlinux.h
                    -c ${item} -o ${BPF_O_FILE}
            COMMAND bash -c "$<TARGET_PROPERTY:bpftool,IMPORTED_LOCATION> gen skeleton ${BPF_O_FILE} > ${BPF_SKEL_FILE}"
            COMMAND_EXPAND_LISTS
            VERBATIM
            DEPENDS ${item})
        add_dependencies(${name} ${item_namewe}_h)
    endforeach(item ${ARGN})
endmacro()
