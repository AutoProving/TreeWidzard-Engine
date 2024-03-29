cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
link_libraries(-lstdc++fs)

if (MSVC)
  # warning level 4 and all warnings as errors
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Werror)
  add_compile_options(-Wall -Wextra -pedantic)
endif()
include_directories(Scripts)
link_libraries(stdc++fs)

project("TreeWidzard-SearchStrategies")

message("Targeting TreeWidzard source at: $ENV{TREEWIDZARD}")

set(CMAKE_SHARED_LIBRARY_PREFIX "")
file(GLOB files
  "Source/*.cpp"
  "Source/**/*.cpp"
)
foreach(file ${files})
  get_filename_component(search_strategy_name "${file}" NAME_WLE)
  message("Adding search strategy: ${search_strategy_name}")
  add_library(${search_strategy_name} SHARED ${file})
  target_include_directories(${search_strategy_name} PRIVATE $ENV{TREEWIDZARD})
endforeach()

