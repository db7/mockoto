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
# installation
################################################################################

install(TARGETS mockoto
        RUNTIME DESTINATION bin)

install(FILES mockoto.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
