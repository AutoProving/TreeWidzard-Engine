cmake_minimum_required(VERSION 3.10)

# Fetch GoogleTest using FetchContent
include(FetchContent)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(INSTALL_GMOCK OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Include test directories
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils)

# Unit test sources
set(UNIT_TEST_SOURCES
    unit/test_bag_operations.cpp
    unit/test_core_support.cpp
    unit/test_kernel_components.cpp
    unit/test_bag_state_fixes.cpp
    unit/test_witnessset_kernel_fixes.cpp
    unit/test_dp_cores.cpp
    unit/test_bfs_multigraph.cpp
    unit/test_dp_cores_correctness.cpp
    unit/test_conjecture_bfs_correctness.cpp
    unit/test_additional_cores.cpp
    unit/test_benchmark_correctness.cpp
)

# Integration test sources
set(INTEGRATION_TEST_SOURCES
    integration/test_atp_workflow.cpp
    integration/test_cli_search.cpp
    integration/test_certificates.cpp
    integration/test_kernel_traversal.cpp
    integration/test_temp_reed.cpp
    integration/test_public_api.cpp
)

# Original test
set(LEGACY_TEST_SOURCES
    test_conjecture.cpp
)

# Create unit test executable
add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests 
    TreeWidzard-Core
    GTest::gtest_main
    ${CMAKE_DL_LIBS}
    Threads::Threads
)

# Create integration test executable
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
target_link_libraries(integration_tests 
    TreeWidzard-Core
    GTest::gtest_main
    ${CMAKE_DL_LIBS}
    Threads::Threads
)

# Integration tests invoke the repo launcher (`treewidzard.sh` on Unix,
# `treewidzard.bat` on Windows), which depends on the main executable and the
# dynamically loaded search strategy plugins being built.
add_dependencies(integration_tests
    treewidzard
    ChromaticNumber_AtMost
    IndependentSet_Max
    MaxDegree_AtLeast
    VertexCount
    BreadthFirstSearch
    IsomorphismBreadthFirstSearch
    ParallelBreadthFirstSearch
    ParallelIsomorphismBreadthFirstSearch
)

# Create legacy test executable (keep existing tests)
add_executable(run_tests ${LEGACY_TEST_SOURCES})
target_link_libraries(run_tests 
    TreeWidzard-Core
    GTest::gtest_main
    ${CMAKE_DL_LIBS}
    Threads::Threads
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(unit_tests
    DISCOVERY_TIMEOUT 60
)

set(TREEWIDZARD_INTEGRATION_TEST_ENV
    "TREEWIDZARD_BUILD_DIR=${PROJECT_BINARY_DIR}"
    "TREEWIDZARD_DPCORES=${PROJECT_BINARY_DIR}/DPCores"
    "TREEWIDZARD_SEARCHSTRATEGIES=${PROJECT_BINARY_DIR}/SearchStrategies"
)
gtest_discover_tests(integration_tests
    DISCOVERY_TIMEOUT 60
    PROPERTIES
        ENVIRONMENT "${TREEWIDZARD_INTEGRATION_TEST_ENV}"
)
gtest_discover_tests(run_tests
    DISCOVERY_TIMEOUT 60
)
