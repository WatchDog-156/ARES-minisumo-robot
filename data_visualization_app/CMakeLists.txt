cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME "PDW-ARES")

project(${PROJECT_NAME} LANGUAGES CXX VERSION 1.0.0)


add_compile_options(-Wall -Wextra)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
set(QtLibs Qt::Core Qt::Widgets)

include_directories(${PROJECT_SOURCE_DIR}/inc)
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/ProsteOkno.cpp
  src/OknoAplikacji.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${QtLibs})
target_link_libraries(${PROJECT_NAME} ${QtLibs})
