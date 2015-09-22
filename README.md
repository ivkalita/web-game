# Web game

### Installation

You need Git installed globally:

```sh
$ git clone https://github.com/kaduev13/web-game
$ cd web-game
$ make deploy
```

C++ libraries:

- googletest framework: libgtest, libgtest_main (for testing only) `sh scripts/install-googletest.sh`
- poco libraries: libPocoNet, libPocoUtil, libPocoJSON, libPocoXML, libPocoFoundation `sh scripts/install-poco.sh`


### Compilation

You need `g++` compiler and `cmake`.



If you are using OS X/Ubuntu, just use "make".

To compile and run server, use

```sh
$ make start
```

To compile and run tests, use

```sh
$ make test
```

