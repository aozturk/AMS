cmake_minimum_required(VERSION 2.8)

project(AMS)

set(AMS_SRCS
  ${PROJECT_SOURCE_DIR}/AMS/IService.cpp
  ${PROJECT_SOURCE_DIR}/AMS/Discovery/Aliveness.cpp
  ${PROJECT_SOURCE_DIR}/AMS/Discovery/Discovery.cpp
  ${PROJECT_SOURCE_DIR}/AMS/Discovery/ReceiverTask.cpp
  ${PROJECT_SOURCE_DIR}/AMS/Discovery/SenderTask.cpp
  ${PROJECT_SOURCE_DIR}/AMS/Net/Socket.cpp
  ${PROJECT_SOURCE_DIR}/AMS/Reactor/Worker.cpp
)

set(AMS_Test_SRCS
  ${PROJECT_SOURCE_DIR}/AMS_test/Main.cpp
)

set(CMAKE_BUILD_TYPE Debug)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions("-std=c++0x")
  add_definitions("-Wall")
endif()
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  add_definitions("-std=c++0x")
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/AMS
  ${PROJECT_SOURCE_DIR}/AMS/Config
  ${PROJECT_SOURCE_DIR}/AMS/Discovery
  ${PROJECT_SOURCE_DIR}/AMS/Net
  ${PROJECT_SOURCE_DIR}/AMS/PubSub
  ${PROJECT_SOURCE_DIR}/AMS/Reactor)

include_directories(
  ${PROJECT_SOURCE_DIR}/AMS_test
  )

include_directories(
  /Users/aozturk/Development/poco/include
  /Users/aozturk/Development/zeromq/include
  /Users/aozturk/Development/msgpack/include
)

link_directories(
  /Users/aozturk/Development/poco/lib
  /Users/aozturk/Development/zeromq/lib
  /Users/aozturk/Development/msgpack/lib
)

add_library(AMS ${AMS_SRCS})

link_directories(${PROJECT_BINARY_DIR})

add_executable(AMS_test ${AMS_Test_SRCS})
target_link_libraries(AMS_test AMS msgpack zmq PocoFoundation PocoNet PocoUtil PocoXML)
