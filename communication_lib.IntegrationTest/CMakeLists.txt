cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include_external_msproject(communication_lib.IntegrationTest
  ${CMAKE_CURRENT_SOURCE_DIR}/communication_lib.IntegrationTest.csproj
  GUID "C48F70C5-2F80-378A-8D2F-CC98C0CC98F7"
)
set_target_properties(communication_lib.IntegrationTest PROPERTIES
  FOLDER "Tests"
)

add_test(
  NAME communication_lib.IntegrationTest
  COMMAND dotnet test
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
