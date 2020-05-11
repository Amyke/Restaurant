cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

add_library(restaurantAdmin-app.lib STATIC)
target_sources(restaurantAdmin-app.lib PRIVATE

  Model.hpp
  Model.cpp

  MainWindow.cpp
  MainWindow.hpp

  LoginWidget.cpp
  LoginWidget.hpp

  WorkspaceWidget.hpp
  WorkspaceWidget.cpp
  OrdersModel.cpp
  OrdersModel.hpp

  OrderDetailsWidget.cpp
  OrderDetailsWidget.hpp
  RequestedFoodModel.cpp
  RequestedFoodModel.hpp

  AdministrationWidget.cpp
  AdministrationWidget.hpp
  FoodModel.cpp
  FoodModel.hpp

  DiagrammWidget.cpp
  DiagrammWidget.hpp
  Charts.hpp
  Charts.cpp
)

target_include_directories(restaurantAdmin-app.lib
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(restaurantAdmin-app.lib
  PUBLIC Qt5::Widgets
         Qt5::Charts
         client_lib
)

set_target_properties(restaurantAdmin-app.lib PROPERTIES
  AUTOMOC ON
  AUTOUIC ON
  FOLDER "Libraries"
)


add_executable(restaurantAdmin-app WIN32)
target_sources(restaurantAdmin-app PRIVATE
  main.cpp
)
target_link_libraries(restaurantAdmin-app
  PRIVATE restaurantAdmin-app.lib
)
set_target_properties(restaurantAdmin-app PROPERTIES
  AUTOMOC ON
  VS_DEBUGGER_ENVIRONMENT "Path=$(Path)\;${Qt5_DIR}/../../../bin"
)

install(
  TARGETS restaurantAdmin-app
  COMPONENT Admin
)
