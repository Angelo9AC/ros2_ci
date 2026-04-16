cmake_minimum_required(VERSION 3.8)
project(fastbot_waypoints)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# Deshabilitar TODOS los linters
set(ament_cmake_copyright_FOUND TRUE)
set(ament_cmake_cppcheck_FOUND TRUE)
set(ament_cmake_cpplint_FOUND TRUE)
set(ament_cmake_flake8_FOUND TRUE)
set(ament_cmake_lint_cmake_FOUND TRUE)
set(ament_cmake_pep257_FOUND TRUE)
set(ament_cmake_uncrustify_FOUND TRUE)
set(ament_cmake_xmllint_FOUND TRUE)
set(ament_lint_auto_FOUND TRUE)

# Generar interfaces
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/Waypoint.action"
  DEPENDENCIES geometry_msgs
)

# Action server
add_executable(fastbot_action_server src/fastbot_action_server.cpp)
ament_target_dependencies(fastbot_action_server
  rclcpp 
  rclcpp_action 
  geometry_msgs
)
target_include_directories(fastbot_action_server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(fastbot_action_server ${PROJECT_NAME}__rosidl_typesupport_cpp)

# Tests
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  
  # Solo compilar nuestros tests, sin linters
  ament_add_gtest(test_waypoints test/test_waypoints.cpp
    TIMEOUT 30
  )
  
  ament_target_dependencies(test_waypoints
    rclcpp 
    rclcpp_action 
    geometry_msgs
  )
  target_include_directories(test_waypoints PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(test_waypoints ${PROJECT_NAME}__rosidl_typesupport_cpp)
endif()

install(TARGETS fastbot_action_server DESTINATION lib/${PROJECT_NAME})
install(TARGETS test_waypoints DESTINATION lib/${PROJECT_NAME})

ament_package()