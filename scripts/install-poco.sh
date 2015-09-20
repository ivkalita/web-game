#!/usr/bin/env bash
wget http://pocoproject.org/releases/poco-1.6.1/poco-1.6.1.tar.gz
tar -xzvf poco-1.6.1.tar.gz
cd poco-1.6.1
./configure && make && make install
cd ..
rm -rf poco-1.6.1*