cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(
    mockoto
    LANGUAGES C CXX)

include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

################################################################################
# clang configuration
################################################################################

find_package(Clang REQUIRED)
find_package(LLVM REQUIRED)

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
              ${CLANG_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}"
)

# Determine if clang-cpp target is available.
# When using clang-cpp, keep linkage minimal to avoid duplicate registration
# of LLVM command line options.
if(TARGET clang-cpp)
  set(LINK_LIBRARIES clang-cpp LLVM)
  message("Linking with clang-cpp target")
else()
  # Fallback for older Clang installs.
  list(APPEND LINK_LIBRARIES LLVM ${CLANG_LIBRARIES})
  list(APPEND LINK_LIBRARIES clangBasic clangAST clangFrontend clangSerialization clangTooling)
  message("Linking with individual Clang libraries")
endif()
# Add LLVM and Clang include directories
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

################################################################################
# mockoto build rules
################################################################################

set(MOCKOTO_VERSIONIZE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/.versionize.sh")
set(MOCKOTO_VERSION_HEADER_IN "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in")
set(MOCKOTO_MANPAGE_IN "${CMAKE_CURRENT_SOURCE_DIR}/mockoto.1.in")
set(MOCKOTO_VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/version.h")
set(MOCKOTO_MANPAGE "${CMAKE_CURRENT_BINARY_DIR}/mockoto.1")

add_custom_command(
    OUTPUT "${MOCKOTO_VERSION_HEADER}"
    COMMAND /bin/sh -c "'${MOCKOTO_VERSIONIZE_SCRIPT}' -r '${MOCKOTO_VERSION_HEADER_IN}' > '${MOCKOTO_VERSION_HEADER}'"
    DEPENDS
      "${MOCKOTO_VERSIONIZE_SCRIPT}"
      "${MOCKOTO_VERSION_HEADER_IN}"
    VERBATIM)

add_custom_command(
    OUTPUT "${MOCKOTO_MANPAGE}"
    COMMAND /bin/sh -c "'${MOCKOTO_VERSIONIZE_SCRIPT}' -r '${MOCKOTO_MANPAGE_IN}' > '${MOCKOTO_MANPAGE}'"
    DEPENDS
      "${MOCKOTO_VERSIONIZE_SCRIPT}"
      "${MOCKOTO_MANPAGE_IN}"
    VERBATIM)

add_custom_target(mockoto_versionized_files
                  DEPENDS "${MOCKOTO_VERSION_HEADER}" "${MOCKOTO_MANPAGE}")

# generate embedable file with binders rkt code
add_custom_target(
    binders
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/binders.rkt > binders.rkt
    COMMAND xxd -i binders.rkt > binders.h
    DEPENDS binders.rkt)

# the tool binary
add_executable(mockoto Mockoto.cpp "${MOCKOTO_VERSION_HEADER}")

# add dependency and path to binders.h header
add_dependencies(mockoto binders mockoto_versionized_files)
target_include_directories(mockoto PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# link to clang and LLVM libraries

target_link_libraries(mockoto PRIVATE ${LINK_LIBRARIES})

################################################################################
# tests
################################################################################

enable_testing()

set(MOCKOTO_TEST_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/tests/run_case.sh")

function(add_mockoto_test CASE_NAME)
  add_test(
      NAME ${CASE_NAME}
      COMMAND sh "${MOCKOTO_TEST_SCRIPT}" "${CASE_NAME}" "$<TARGET_FILE:mockoto>"
              "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

add_mockoto_test(c_mode_generates_helpers)
add_mockoto_test(c_mode_void_has_no_returns_helper)
add_mockoto_test(c_mode_pointer_return_has_returns_helper)
add_mockoto_test(c_mode_function_pointer_param_signature)
add_mockoto_test(h_mode_generates_header_api)
add_mockoto_test(default_mode_is_c)
add_mockoto_test(print_src_reports_locations)
add_mockoto_test(path_exclude_filters_decls)
add_mockoto_test(static_and_extern_are_ignored)
add_mockoto_test(multi_header_generates_all_funcs)
add_mockoto_test(rkt_mode_generates_bindings)
add_mockoto_test(chibi_mode_generates_stub)
add_mockoto_test(unknown_mode_fails)
add_mockoto_test(version_flags_show_version)
add_mockoto_test(compile_and_run_generated_mock)
add_mockoto_test(optional_racket_generation_runtime)

set_tests_properties(optional_racket_generation_runtime
                     PROPERTIES SKIP_RETURN_CODE 77)

################################################################################
# installation
################################################################################

install(TARGETS mockoto
        RUNTIME DESTINATION bin)

install(FILES "${MOCKOTO_MANPAGE}"
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
