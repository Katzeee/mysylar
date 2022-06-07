cmake_minimum_required(VERSION 3.22)
project(mysylar)
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED 17) 
set(CMAKE_GENERATOR Makefiles)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

find_package(yaml-cpp CONFIG REQUIRED)


add_subdirectory(src)

add_subdirectory(tests)



