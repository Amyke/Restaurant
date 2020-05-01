cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

add_executable(client_lib.test.protocol)
target_sources(client_lib.test.protocol PRIVATE
  protocol_test.cpp
)

target_link_libraries(client_lib.test.protocol
  PRIVATE client_lib
          Qt5::Test
)
set_target_properties(client_lib.test.protocol PROPERTIES
  AUTOMOC ON
  VS_DEBUGGER_ENVIRONMENT "Path=$(Path)\;${Qt5_DIR}/../../../bin"
)

add_test(
  NAME client_lib.test.protocol
  COMMAND client_lib.test.protocol
)
set_tests_properties(client_lib.test.protocol PROPERTIES
  ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin\;$ENV{PATH}"
)
