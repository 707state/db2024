set(SOURCES disk_manager.cpp buffer_pool_manager.cpp ../replacer/replacer.h
            ../replacer/lru_replacer.cpp)

add_library(storage OBJECT ${SOURCES})

target_link_libraries(storage stdc++fs)
