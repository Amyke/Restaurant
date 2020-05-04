cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

add_qt_test(restaurantAdmin-app.test.model
  model_test.cpp
)
target_link_libraries(restaurantAdmin-app.test.model
  PRIVATE restaurantAdmin-app.lib
)

add_qt_test(restaurantAdmin-app.test.foodModel
  foodmodel_test.cpp
)
target_link_libraries(restaurantAdmin-app.test.foodModel
  PRIVATE restaurantAdmin-app.lib
)

add_qt_test(restaurantAdmin-app.test.orderModel
  ordermodel_test.cpp
)
target_link_libraries(restaurantAdmin-app.test.orderModel
  PRIVATE restaurantAdmin-app.lib
)