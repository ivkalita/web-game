# Web game

## Installation

You have to install PostgreSQL 9.4
Key files for project are: libpq-fe.h and libpq.lib

If you install for Visual Studio you should:
- install 32bit version of PostgreSQL
- add libpq.lib to Linker->Input->Additional dependencies
- add <pgsql install path>\include and \lib to VC++ Directories->Include and ->Library, correspondingly

You should create your own local PostgreSQL database server
Then you can use sql/database.sql file for creating and filling your database

Required C++ libraries (they will be installed automatically):

- googletest framework: libgtest, libgtest_main (for testing only)
- poco libraries: libPocoNet, libPocoUtil, libPocoJSON, libPocoXML, libPocoFoundation

### Ubuntu/Mac OS X

You need Git installed globally, g++ compiler and cmake (v3+).

```sh
$ git clone https://github.com/kaduev13/web-game
$ cd web-game
$ git submodule update --init
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


