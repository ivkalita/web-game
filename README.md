# Web game

[![Build Status](https://travis-ci.org/kaduev13/web-game.svg?branch=master)](https://travis-ci.org/kaduev13/web-game)

### Installation

You need Git installed globally:

```sh
$ git clone https://github.com/kaduev13/web-game
```

For tests you need google test framework dynamic libraries installed globally. For linux/Mac OS X you can install them by using `scripts/install-googletest.sh`. 

### Compilation

You need g++ compiler.

If you are using OS X/Ubuntu, just use "make". You should run `./configure` only one time (it'll create some additional folders and configs).

To compile and run program, use

```sh
$ make
$ ./bin/runner
```

To compile and run tests, use

```sh
$ make tester
$ ./bin/tester
```

To run tests, use

```sh
$ make test
```

