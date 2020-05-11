cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

include_external_msproject(restaurant-server
  ${CMAKE_CURRENT_SOURCE_DIR}/restaurant-server.csproj
  GUID "0F529FDC-6FCA-34B5-85C0-BFB66A8D7E84"
)

add_custom_target(restaurant-server.publish ALL
  COMMAND dotnet publish
          --configuration $<CONFIG>
          --self-contained true
          -p:PublishSingleFile=true
          --output ${CMAKE_CURRENT_BINARY_DIR}/publish/$<CONFIG>
          --runtime $<$<PLATFORM_ID:Windows>:win-x64>$<$<PLATFORM_ID:Linux>:linux-x64>
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

install(
  PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/publish/$<CONFIG>/restaurant-server$<$<PLATFORM_ID:Windows>:.exe>
  TYPE BIN
  COMPONENT Server
)
