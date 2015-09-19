#!/usr/bin/env bash
wget http://googletest.googlecode.com/files/gtest-1.7.0.zip
unzip gtest-1.7.0.zip
export LD_LIBRARY_PATH=/usr/lib/
cd gtest-1.7.0 && ./configure && make
sudo cp -a lib/.libs/* /usr/lib/
cd ..
rm -rf gtest-1.7.0*