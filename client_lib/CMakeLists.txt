cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

add_library(client_lib)
target_sources(client_lib PRIVATE

  Client.hpp
  Client.cpp

  Protocol.hpp
  Protocol.cpp

  Message.hpp
  Message.cpp
)

target_include_directories(client_lib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(client_lib
  PUBLIC msgpackc-cxx
  PRIVATE Qt5::Network
)
set_target_properties(client_lib PROPERTIES
  AUTOMOC ON
  AUTOUIC ON
)
