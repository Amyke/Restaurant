cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BeginOfDeath VERSION 0.1.0 LANGUAGES CXX)

include(FeatureSummary)

find_package(msgpack 3.2.1 EXACT REQUIRED CONFIG)
find_package(Qt5 5.12 REQUIRED CONFIG
  COMPONENTS Widgets
             Network
             Test
)

add_compile_definitions(
  NOMINMAX
  SERVER_PORT=9012
)

enable_testing()

if(CMAKE_GENERATOR MATCHES "^Visual Studio .*$")
  add_subdirectory(communication_lib)
  add_subdirectory(server)
  add_subdirectory(communication_lib.test)
  add_subdirectory(communication_lib.IntegrationTest)
endif()

add_subdirectory(client_lib)
add_subdirectory(client_lib.test)
add_subdirectory(restaurant-app)
add_subdirectory(restaurant-app.test)
add_subdirectory(admin_client)

feature_summary(WHAT ALL)
