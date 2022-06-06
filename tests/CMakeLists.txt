add_executable(logtest logtest.cc)
add_dependencies(logtest sylar)
target_link_libraries(logtest sylar)

add_executable(configtest configtest.cc)
add_dependencies(configtest sylar)
target_link_libraries(configtest sylar)
