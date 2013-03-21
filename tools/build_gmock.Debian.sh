#!/bin/bash
g++ -I/usr/src/gmock -c /usr/src/gmock/src/gmock-all.cc
g++ -I/usr/src/gtest -c /usr/src/gtest/src/gtest-all.cc
ar -rv libgmock.a gmock-all.o gtest-all.o
mkdir gmock
mv libgmock.a gmock
cd gmock
ln -s libgmock.a libgtest.a
