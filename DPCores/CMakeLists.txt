cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
link_libraries(-lstdc++fs)

if (MSVC)
  # warning level 4 and all warnings as errors
  add_compile_options(/W4 /WX)
else()
  # add_compile_options(-Werror)
  add_compile_options(-Wall -Wextra -pedantic)
endif()
add_compile_options(-O3)
include_directories(Scripts)
link_libraries(stdc++fs)

#add_compile_options(-fsanitize=address)
#add_compile_options(-fsanitize=undefined)
#add_link_options(-fsanitize=address)
#add_link_options(-fsanitize=undefined)

project("TreeWidzard-DPCores")

message("Targeting TreeWidzard source at: $ENV{TREEWIDZARD}")

set(CMAKE_SHARED_LIBRARY_PREFIX "")
file(GLOB files "Source/*.cpp")
foreach(file ${files})
  get_filename_component(core_name "${file}" NAME_WLE)
  message("Adding core: ${core_name}")
  add_library(${core_name} SHARED ${file})
  target_include_directories(${core_name} PRIVATE $ENV{TREEWIDZARD})
endforeach()

