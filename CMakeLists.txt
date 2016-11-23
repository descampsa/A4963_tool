cmake_minimum_required(VERSION 3.0)
project(A4963_tool)

find_package(wxWidgets REQUIRED core base)
include(${wxWidgets_USE_FILE})

add_executable(A4963_tool main.cpp A4963_tool.cpp A4963_config.cpp controller.cpp)
target_link_libraries(A4963_tool ${wxWidgets_LIBRARIES} serialport)
