aux_source_directory(. LIB_SRC)

add_library(sylar SHARED ${LIB_SRC})


target_link_libraries(sylar yaml-cpp)