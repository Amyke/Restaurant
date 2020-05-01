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

install(
  TARGETS restaurant-app
)
install(CODE "
find_program(_windeployqt
  NAME windeployqt
  PATHS $<TARGET_FILE_DIR:Qt5::Core>
  NO_SYSTEM_PATHS
)

execute_process(
  COMMAND \${_windeployqt}
          --$<IF:$<CONFIG:Debug>,debug,release>
          --no-translations
          --no-compiler-runtime
          --no-opengl-sw
          --no-system-d3d-compiler
          --dir \${CMAKE_INSTALL_PREFIX}/bin
          $<TARGET_FILE:restaurant-app>
)
")
