# "test" is reserved by CTest as a target name, so the target is exec_test.
add_executable(exec_test main.cc)

target_link_libraries(exec_test PRIVATE nasdaq_itch::itch)
