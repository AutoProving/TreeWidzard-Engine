cmake_minimum_required(VERSION 3.18)
project(TreeWidzardInstallSmoke LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED TREEWIDZARD_INSTALL_PREFIX)
  message(FATAL_ERROR "TREEWIDZARD_INSTALL_PREFIX must point to an installed TreeWidzard prefix")
endif()

find_library(TREEWIDZARD_CORE_LIBRARY
  NAMES TreeWidzard-Core
  PATHS
    "${TREEWIDZARD_INSTALL_PREFIX}/lib"
    "${TREEWIDZARD_INSTALL_PREFIX}/lib64"
  NO_DEFAULT_PATH
  REQUIRED)

add_executable(treewidzard_install_smoke main.cpp)
target_include_directories(treewidzard_install_smoke PRIVATE
  "${TREEWIDZARD_INSTALL_PREFIX}/include/treewidzard"
  "${TREEWIDZARD_INSTALL_PREFIX}/include/treewidzard/internal")
target_link_libraries(treewidzard_install_smoke PRIVATE "${TREEWIDZARD_CORE_LIBRARY}")

if(UNIX)
  get_filename_component(TREEWIDZARD_CORE_LIBRARY_DIR "${TREEWIDZARD_CORE_LIBRARY}" DIRECTORY)
  set_target_properties(treewidzard_install_smoke PROPERTIES
    BUILD_RPATH "${TREEWIDZARD_CORE_LIBRARY_DIR}")
endif()
