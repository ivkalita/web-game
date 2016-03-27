cmake_minimum_required(VERSION 2.8)
project(web_game)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
include_directories(include)

#Exception support
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
elseif (MSVC71 OR MSVC80)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHc-")
endif()

#PostgreSQL
set(PostgreSQL_ADDITIONAL_VERSIONS "9.4" "9.3")
set(PostgreSQL_ADDITIONAL_SEARCH_PATHS "/usr/include/postgresql/9.4/server" "/usr/include/postgresql/9.3/server")
FIND_PACKAGE(PostgreSQL)
if(PostgreSQL_FOUND)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
    link_directories(${PostgreSQL_LIBRARY_DIRS})
else()
    message(SEND_ERROR "PostgreSQL not found.")
endif()

#Use shared crt instead of static
if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

#external libraries
set(gtest_force_shared_crt ON CACHE BOOL "Use shared (DLL) run-time lib even when Google Test is built as static lib." FORCE)
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
    src/WebgameServer.cpp
    src/AbstractRouter.cpp
    src/Router.cpp
    src/Pages.cpp
    src/GameEngine.cpp
    src/GameEngine_Ship.cpp
    src/GameEngine_ShipGroup.cpp
    src/GameEngine_Planet.cpp
    src/Users.cpp
    src/GameHandler.cpp
    src/Map.cpp
    src/MatchmakingHandler.cpp
    include/GameEngine.hpp
    include/DBConnector.hpp
    include/Router.hpp
    include/WebgameServer.hpp
    include/User.hpp
    include/MathVector.hpp
    include/Map.hpp
)
target_link_libraries(runner ${POCO_LIBS} ${PostgreSQL_LIBRARIES})

add_executable(tester
    src/AbstractRouter.cpp
    src/GameEngine.cpp
    src/GameEngine_Ship.cpp
    src/GameEngine_ShipGroup.cpp
    src/GameEngine_Planet.cpp
    tests/Engine/EngineTest1.cpp
    tests/test_1.cpp
    tests/DBConnector/test_1.cpp
    tests/Router/test_1.cpp
)
target_link_libraries(tester ${POCO_LIBS} ${GTEST_LIBS} ${PostgreSQL_LIBRARIES})
