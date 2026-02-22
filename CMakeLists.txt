cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(
    mockoto
    LANGUAGES C CXX
    VERSION 0.1)

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

list(APPEND LINK_LIBRARIES LLVM ${CLANG_LIBRARIES})

# Determine if clang-cpp target is available
if(TARGET clang-cpp)
  list(APPEND LINK_LIBRARIES clang-cpp)
  message("Linking with clang-cpp target")
else()
  # For older versions, manually add necessary Clang libraries
  list(APPEND LINK_LIBRARIES clangBasic clangAST clangFrontend clangSerialization clangTooling)
  message("Linking with individual Clang libraries")
endif()
# Add LLVM and Clang include directories
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

################################################################################
# mockoto build rules
################################################################################

# generate embedable file with binders rkt code
add_custom_target(
    binders
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/binders.rkt > binders.rkt
    COMMAND xxd -i binders.rkt > binders.h
    DEPENDS binders.rkt)

# the tool binary
add_executable(mockoto Mockoto.cpp)

# add dependency and path to binders.h header
add_dependencies(mockoto binders)
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
add_mockoto_test(compile_and_run_generated_mock)
add_mockoto_test(optional_chibi_generation_runtime)
add_mockoto_test(optional_racket_generation_runtime)

set_tests_properties(optional_chibi_generation_runtime
                     PROPERTIES SKIP_RETURN_CODE 77)
set_tests_properties(optional_racket_generation_runtime
                     PROPERTIES SKIP_RETURN_CODE 77)

################################################################################
# installation
################################################################################

install(TARGETS mockoto
        RUNTIME DESTINATION bin)

install(FILES mockoto.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
