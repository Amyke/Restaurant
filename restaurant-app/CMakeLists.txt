cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

add_library(restaurant-app.lib STATIC)
target_sources(restaurant-app.lib PRIVATE
  MainWidget.hpp
  MainWidget.cpp

  LoginWidget.hpp
  LoginWidget.cpp

  WelcomeWidget.hpp
  WelcomeWidget.cpp

  OrderWidget.hpp
  OrderWidget.cpp
  MenuModel.hpp
  MenuModel.cpp
  CartModel.hpp
  CartModel.cpp

  PayWidget.hpp
  PayWidget.cpp
  OrderModel.hpp
  OrderModel.cpp

  EndWidget.hpp
  EndWidget.cpp

  Model.hpp
  Model.cpp
)
target_include_directories(restaurant-app.lib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(restaurant-app.lib
  PUBLIC Qt5::Widgets
         client_lib
)
set_target_properties(restaurant-app.lib PROPERTIES
  AUTOMOC ON
  AUTOUIC ON
)

add_executable(restaurant-app WIN32)
target_sources(restaurant-app PRIVATE
  main.cpp
)
target_link_libraries(restaurant-app
  PRIVATE restaurant-app.lib
)
set_target_properties(restaurant-app PROPERTIES
  AUTOMOC ON
  VS_DEBUGGER_ENVIRONMENT "Path=$(Path)\;${Qt5_DIR}/../../../bin"
)
