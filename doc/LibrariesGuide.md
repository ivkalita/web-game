# External library installation guide

If you want to include some external library to this project, you should:

* Create git submodule with this library in folder `lib/`. For example (here and below I'll demonstrate you how to include googletest library):

```sh
$ pwd
$ ~/Documents/web-game
$ cd lib
$ git submodule add https://github.com/google/googletest.git gtest
```
It will create folder `lib/gtest`, contains library sources and modify .gitmodules file.

* Add subdirectory to `CMakeLists.txt`

```
add_subdirectory(lib/gtest/googletest) #googletest has 2 libraries (googletest and googlemock) in their repository, so I specify googletest
```

* Add include directories of external library to `CMakeLists.txt`

```
include_directories(${gtest_SOURCE_DIR}/include)
```

* (Optional) Define your variable in `CMakeLists.txt` with set of needed external libraries.

```
#here I need gtest and gtest_main libraries
set(GTEST_LIBS
    gtest
    gtest_main
)
```

* Link target with libraries in `CMakeLists.txt`

```
target_link_libraries(runner ${GTEST_LIBS})
```