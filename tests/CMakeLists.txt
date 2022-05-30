add_executable(logtest logtest.cc)
add_dependencies(logtest sylar)
target_link_libraries(logtest sylar)

