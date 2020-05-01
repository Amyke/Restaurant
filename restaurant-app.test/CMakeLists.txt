cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

add_executable(restaurant-app.test.model)
target_sources(restaurant-app.test.model PRIVATE
  model_test.cpp
)

target_link_libraries(restaurant-app.test.model
  PRIVATE Qt5::Test
          restaurant-app.lib
)

set_target_properties(restaurant-app.test.model PROPERTIES
  AUTOMOC ON
  VS_DEBUGGER_ENVIRONMENT "Path=$(Path)\;${Qt5_DIR}/../../../bin"
)

add_test(
  NAME restaurant-app.test.model
  COMMAND restaurant-app.test.model
)

set_tests_properties(restaurant-app.test.model PROPERTIES
  ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin\;$ENV{PATH}"
)
