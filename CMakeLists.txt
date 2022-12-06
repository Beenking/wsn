cmake_minimum_required(VERSION 3.1)

project(WSN)

find_package(OpenCV REQUIRED)

add_executable(wsn_exe main.cpp wsnnode.cpp wsnnode.h wsntopologybasedenergy.h wsntopologybasedenergy.cpp)
target_link_libraries(wsn_exe ${OpenCV_LIBS})
target_include_directories(wsn_exe PUBLIC ${OpenCV_INCLUDE_DIRS})