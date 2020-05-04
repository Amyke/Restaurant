cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

add_qt_test(restaurant-app.test.model
  model_test.cpp
)
target_link_libraries(restaurant-app.test.model
  PRIVATE restaurant-app.lib
)
