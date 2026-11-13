# "test" is reserved by CTest as a target name, so the target is exec_test.
add_executable(exec main.cc)

target_link_libraries(exec PRIVATE nasdaq_itch::itch)
