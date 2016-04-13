# Web game

## Configuration

Set environment variable WEBGAME_BIN_FOLDER as an absolute path to the application's bin folder **WITHOUT TRAILING SLASH**

**Unix**

For shell session:

`export WEBGAME_BIN_FOLDER="/home/user/web-game/bin"`

Permanent:

`echo 'export WEBGAME_BIN_FOLDER="/home/user/web-game/bin"' >> ~/.profile`

**Windows**

For shell session:

`set WEBGAME_BIN_FOLDER=D:/web-game/bin`

Permanent:

Control Panel -> User accounts -> Environment Variables -> Create

All config files located at `./bin` folder, and have extension `.ini`. First time you clonned repository,
 you will see files like `*.ini.dist` in `./bin` folder. Copy them with new names, without `.dist` suffix
 (for example, copy `./bin/runner.ini.dist` to `./bin/runner.ini`). Now you can setup your own configuration
 in `./bin/*.ini` files (for example, database username).
 It is important to setup `rootpath` property in configuration - it is full path to your project directory.

## Installation

You have to install PostgreSQL 9.4
Key files for project are: libpq-fe.h and libpq.lib (libpq.a)

You should create your own local PostgreSQL database server
Then you can use sql/database.sql file for creating and filling your database

Required C++ libraries (they will be installed automatically):

- googletest framework: libgtest, libgtest_main (for testing only)
- poco libraries: libPocoNet, libPocoUtil, libPocoJSON, libPocoXML, libPocoFoundation

### Ubuntu/Mac OS X

You need Git installed globally, g++ compiler, PostgreSQL and cmake (v3+).

```sh
$ git clone https://github.com/kaduev13/web-game
$ cd web-game
$ git submodule update --init
$ cmake .
$ make runner
$ make tester
```

#### Known issues

If you will get problems with libpq-fe.h, than make sure, that libpq-fe.h exists (check `/usr/include`, `/usr/local/include`, `/usr/include/postgresql`, `/usr/local/include/postgresql`).
Also, you compiler should know about include path for libpq-fe.h. Look through environment variables like
`CPLUS_INCLUDE_PATH` and `C_INCLUDE_PATH`.

### Windows

If you install for Visual Studio you should:
- install 32bit version of PostgreSQL
- add libpq.lib to Linker->Input->Additional dependencies
- add <pgsql install path>\include and \lib to VC++ Directories->Include and ->Library, correspondingly

### Bower installation
We are using bower package manager so you should download it.
You should install Node.js first. After installation open command line and use next command:

```sh
$ npm install -g bower
```
This will automatically install and set PATH variables for Bower. After this, open `/web-game` folder with a command line and init next command
`bower install`. This will download all libs we are using in `/web-game/web/assets/js/vendor`.

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


