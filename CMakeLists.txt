cmake_minimum_required(VERSION 2.8)
project(web_game)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
include_directories(include)

#Exception support
if (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
elseif (MSVC71 OR MSVC80)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHc-")
endif()

#PostgreSQL
if (WIN32)
	FIND_PACKAGE(PostgreSQL)
	if(PostgreSQL_FOUND)
		include_directories(${PostgreSQL_INCLUDE_DIRS})
		link_directories(${PostgreSQL_LIBRARY_DIRS})
	else()
		message(SEND_ERROR "PostgreSQL not found.")
	endif()
	set(PQ_LIBS ${PostgreSQL_LIBRARIES}})
else()
	set(PQ_LIBS pq)
endif()

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

if (WIN32)
set(PQ_LIBS libpq)
else()
set(PQ_LIBS pq)				#??? Check library name on non windows platforms.
endif()

add_executable(runner
    src/main.cpp
    src/WebSocketServer.cpp)
target_link_libraries(runner ${POCO_LIBS} ${PQ_LIBS})

add_executable(tester
    tests/test_1.cpp)
target_link_libraries(tester ${POCO_LIBS} ${GTEST_LIBS} ${PQ_LIBS})