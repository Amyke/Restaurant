cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include_external_msproject(restaurant-server.test
  ${CMAKE_CURRENT_SOURCE_DIR}/restaurant-server.test.csproj
  GUID "9356D7D1-0F1B-3B2D-8FC2-016CEB595F8A"
)
set_target_properties(restaurant-server.test PROPERTIES
  FOLDER "Tests"
)

add_test(
  NAME restaurant-server.test
  COMMAND dotnet test
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
