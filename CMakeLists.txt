cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(
    mockoto
    LANGUAGES C CXX
    VERSION 0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

################################################################################
# clang configuration
################################################################################

find_package(Clang REQUIRED)
message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
              ${CLANG_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}"
)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

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

# link to clang libraries
target_link_libraries(mockoto
  PRIVATE
  clangAST
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
)

################################################################################
# installation
################################################################################

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/mockoto
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin)
