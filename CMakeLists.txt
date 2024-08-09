cmake_minimum_required(VERSION 3.26)

project(vvm) 
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
  src/Program.cpp
  src/VM.cpp
)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES}) 
add_library(lib${PROJECT_NAME} STATIC ${SOURCES})
