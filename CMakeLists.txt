cmake_minimum_required(VERSION 3.12)

project("cube")

set(SRC_FILES
  src/main.cpp
  src/window.cpp
  src/window.h
  src/cube.cpp
  src/cube.h
  src/timer.cpp
  src/timer.h
  src/utils.cpp
  src/utils.h)

add_executable(cube ${SRC_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE TRUE)

# target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE)

target_link_libraries(${PROJECT_NAME} D3D12.lib)
