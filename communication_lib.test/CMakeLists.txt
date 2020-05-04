cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include_external_msproject(communication_lib.test
  ${CMAKE_CURRENT_SOURCE_DIR}/communication_lib.test.csproj
  GUID "2703BE8D-A3CA-3A15-99E3-C7F0A1BA5EA2"
)
set_target_properties(communication_lib.test PROPERTIES
  FOLDER "Tests"
)

add_test(
  NAME communication_lib.test
  COMMAND dotnet test
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
