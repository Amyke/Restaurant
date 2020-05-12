cmake_minimum_required(VERSION 3.17 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Restaurant VERSION 0.1.0 LANGUAGES CXX)

set_property(GLOBAL PROPERTY
  USE_FOLDERS ON
)
set_property(GLOBAL PROPERTY
  PREDEFINED_TARGETS_FOLDER "CMake"
)

include(FeatureSummary)

find_package(msgpack 3.2.1 EXACT REQUIRED CONFIG)
find_package(Qt5 5.11 REQUIRED CONFIG
  COMPONENTS Widgets
             Network
             Test
             Charts
)

add_compile_definitions(
  NOMINMAX
  SERVER_PORT=9012
)

function(add_dotnet_target _name _project_file _guid)
  if(CMAKE_GENERATOR MATCHES "^Visual Studio .*$")
    include_external_msproject(${_name} "${_project_file}"
      GUID "${_guid}"
    )
  else()
    file(GLOB_RECURSE _sources CONFIGURE_DEPENDS "*.cs")
    add_custom_target(${_name} ALL
      COMMAND dotnet build --configuration $<CONFIG>
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      SOURCES ${_sources}
      DEPENDS ${_sources}
    )
  endif()
endfunction()

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

add_subdirectory(communication_lib)
add_subdirectory(communication_lib.test)
add_subdirectory(communication_lib.IntegrationTest)

add_subdirectory(server)
add_subdirectory(restaurant-server.test)

add_subdirectory(client_lib)
add_subdirectory(client_lib.test)

add_subdirectory(restaurant-app)
add_subdirectory(restaurant-app.test)

add_subdirectory(admin_client)
add_subdirectory(admin_client.test)

feature_summary(WHAT ALL)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Beke Ágnes")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION "hosszú leírás")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "rövid leírás")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
if(WIN32)
  set(CPACK_GENERATOR "IFW")
  set(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY "${PROJECT_NAME}")
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
              $<TARGET_FILE:restaurantAdmin-app>
    )
    "
    COMPONENT Qt
  )
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
  include(InstallRequiredSystemLibraries)
  install(
    PROGRAMS ${MSVC_REDIST_DIR}/vc_redist.x64.exe
    DESTINATION vc_redist
    COMPONENT vc_redist
  )
else()
endif()
include(CPack)

set(_Admin_DISPLAY_NAME "Admin")
set(_Admin_DESCRIPTION "${PROJECT_NAME} Administrator client")
set(_Customer_DISPLAY_NAME "Customer")
set(_Customer_DESCRIPTION "${PROJECT_NAME} Customer client")
if(WIN32)
  cpack_add_component(Qt HIDDEN)
  cpack_add_component(vc_redist HIDDEN)
  cpack_add_component(Admin
    DISPLAY_NAME "${_Admin_DISPLAY_NAME}"
    DESCRIPTION "${_Admin_DESCRIPTION}"
    DEPENDS Qt vc_redist
  )
  cpack_add_component(Customer
    DISPLAY_NAME "${_Customer_DISPLAY_NAME}"
    DESCRIPTION "${_Customer_DESCRIPTION}"
    DEPENDS Qt vc_redist
  )
else()
  cpack_add_component(Admin
    DISPLAY_NAME "${_Admin_DISPLAY_NAME}"
    DESCRIPTION "${_Admin_DESCRIPTION}"
  )
  cpack_add_component(Customer
    DISPLAY_NAME "${_Customer_DISPLAY_NAME}"
    DESCRIPTION "${_Customer_DESCRIPTION}"
  )
endif()
cpack_add_component(Server
  DISPLAY_NAME "Server"
  DESCRIPTION "${PROJECT_NAME} Server"
)

if(WIN32)
  include(CPackIFW)
  cpack_ifw_configure_component(Admin
    SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/packaging/Admin.qs
  )
  cpack_ifw_configure_component(Customer
    SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/packaging/Customer.qs
  )
  cpack_ifw_configure_component(vc_redist
    SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/packaging/vc_redist.qs
  )
  cpack_ifw_configure_component(server
    REQUIRES_ADMIN_RIGHTS
    SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/packaging/Server.qs
  )
endif()
