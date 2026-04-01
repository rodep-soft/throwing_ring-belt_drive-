cmake_minimum_required(VERSION 3.8)
project(launcher)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
#find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_srvs REQUIRED)
find_package(sensor_msgs REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
#rosidl_generate_interfaces(launcher
#	"srv/ExecutionCommand.srv"
#	DEPENDENCIES sensor_msgs
#)

add_executable(launcher_node src/launcher_node.cpp)
ament_target_dependencies(launcher_node 
	rclcpp 
	sensor_msgs
	std_srvs)

# 重要：生成されたメッセージヘッダーを使えるようにするおまじない
#rosidl_get_typesupport_target(cpp_typesupport_target "launcher" "rosidl_typesupport_cpp")
#target_link_libraries(launcher_node "${cpp_typesupport_target}")

install(TARGETS
	launcher_node
	DESTINATION lib/launcher
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
