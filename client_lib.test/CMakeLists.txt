cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

add_qt_test(client_lib.test.protocol
  protocol_test.cpp
)
target_link_libraries(client_lib.test.protocol
  PRIVATE client_lib
)
