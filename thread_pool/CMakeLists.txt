project(thread_pool)
set(CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")  #std::thread  must be to linux   
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(main
  ${CMAKE_CURRENT_LIST_DIR}/test.cpp
)
set_property(TARGET main PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/thread_pool)

