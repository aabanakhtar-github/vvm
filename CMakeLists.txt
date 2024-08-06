project(vvm) 
cmake_minimum_required(VERSION 3.26)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
  src/main.cpp
  src/Program.cpp
  src/VM.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES}) 
