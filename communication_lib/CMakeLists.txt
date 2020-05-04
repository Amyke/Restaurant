cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include_external_msproject(communication_lib
  ${CMAKE_CURRENT_SOURCE_DIR}/communication_lib.csproj
  GUID "A9A39ED6-1305-3357-B4B2-0125EDFD8227"
)
set_target_properties(communication_lib PROPERTIES
  FOLDER "Libraries"
)
