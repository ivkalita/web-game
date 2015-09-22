# Web game

## Installation

Required C++ libraries (they will be installed automatically):

- googletest framework: libgtest, libgtest_main (for testing only) `sh scripts/install-googletest.sh`
- poco libraries: libPocoNet, libPocoUtil, libPocoJSON, libPocoXML, libPocoFoundation `sh scripts/install-poco.sh`

### Ubuntu/Mac OS X

You need Git installed globally, g++ compiler and cmake (v3+).

```sh
$ git clone https://github.com/kaduev13/web-game
$ cd web-game
$ git submodule init
$ git submodule update
$ cmake .
$ make runner
$ make tester
```

### Windows

???

## Running

```sh
$ ./bin/runner
```

## Testing

```sh
$ ./bin/tester
```

## Contributing

Read this articles first.

[Collaborator's guide](https://github.com/kaduev13/web-game/wiki/Collaborator's-guide)

[Libraries guide](doc/LibrariesGuide.md)


