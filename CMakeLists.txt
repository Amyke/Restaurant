cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BeginOfDeath VERSION 0.1.0 LANGUAGES CXX)

set_property(GLOBAL PROPERTY
  USE_FOLDERS ON
)
set_property(GLOBAL PROPERTY
  PREDEFINED_TARGETS_FOLDER "CMake"
)

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
function(add_qt_test _name _source)
  add_executable(${_name})
  target_sources(${_name} PRIVATE ${_source})
  target_link_libraries(${_name} PRIVATE Qt5::Test)
  set_target_properties(${_name} PROPERTIES
    AUTOMOC ON
    FOLDER "Tests"
    VS_DEBUGGER_ENVIRONMENT "Path=$(Path)\;${Qt5_DIR}/../../../bin"
  )

  add_test(
    NAME ${_name}
    COMMAND ${_name}
  )
  set_tests_properties(${_name} PROPERTIES
    ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin\;$ENV{PATH}"
  )
endfunction()

if(CMAKE_GENERATOR MATCHES "^Visual Studio .*$")
  add_subdirectory(communication_lib)
  add_subdirectory(communication_lib.test)
  add_subdirectory(communication_lib.IntegrationTest)

  add_subdirectory(server)
endif()

add_subdirectory(client_lib)
add_subdirectory(client_lib.test)

add_subdirectory(restaurant-app)
add_subdirectory(restaurant-app.test)

add_subdirectory(admin_client)
add_subdirectory(admin_client.test)

feature_summary(WHAT ALL)
