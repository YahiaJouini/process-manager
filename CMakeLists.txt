cmake_minimum_required(VERSION 3.16)
project(procmanager)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directory for your headers
include_directories(include)

# Fetch FTXUI automatically
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG main
)
FetchContent_MakeAvailable(ftxui)

# Collect all .cpp files recursively
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  src/*.cpp
)

# Define the executable
add_executable(procmanager ${SRC_FILES})

# Link with FTXUI
target_link_libraries(procmanager PRIVATE
  ftxui::screen
  ftxui::dom
  ftxui::component
)

# Optionally, ensure include/ is recognized as public headers
target_include_directories(procmanager PRIVATE ${CMAKE_SOURCE_DIR}/include)
