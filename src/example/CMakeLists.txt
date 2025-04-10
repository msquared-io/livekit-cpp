cmake_minimum_required(VERSION 3.15)
project(example)

add_executable(example
    example.cpp
    example.hpp
)

target_include_directories(example
    PRIVATE ${CMAKE_INSTALL_PREFIX}/include
)

target_link_directories(example
    PRIVATE ${CMAKE_INSTALL_PREFIX}/lib
)

target_link_libraries(example
    PRIVATE livekit
)

install(TARGETS example
    RUNTIME DESTINATION bin
)