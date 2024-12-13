cmake_minimum_required(VERSION 3.12)

project("cube")

set(SRC_FILES
  src/main.cpp
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

if (WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE "/wd4819")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE PRIVATE _UNICODE)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include/directx)

target_link_libraries(${PROJECT_NAME} D3D12.lib dxgi.lib)
