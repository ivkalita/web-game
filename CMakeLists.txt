cmake_minimum_required(VERSION 2.8)
project(web_game)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
include_directories(include)

#external libraries
add_subdirectory(lib/gtest/googletest)
include_directories(${gtest_SOURCE_DIR}/include)
set(GTEST_LIBS
    gtest
    gtest_main
)

add_subdirectory(lib/poco)
include_directories(${poco_SOURCE_DIR}/include)
set(POCO_LIBS
    PocoUtil
    PocoNet
    PocoXML
    PocoJSON
    PocoFoundation
)
#targets
#specify source files by listing them explicitly
#http://stackoverflow.com/a/18538444/3838486
add_executable(runner
    src/main.cpp
    src/WebSocketServer.cpp)
target_link_libraries(runner ${POCO_LIBS} pq)

add_executable(tester
    tests/test_1.cpp)
target_link_libraries(tester ${POCO_LIBS} ${GTEST_LIBS} pq)