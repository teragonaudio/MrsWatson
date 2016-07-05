#!/bin/bash

TRAVIS_OS_NAME=$1

case $TRAVIS_OS_NAME in
  osx)
    brew update
    brew install cmake ninja libzip
    ;;
  linux)
    sudo dpkg --add-architecture i386
    wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    echo "deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-3.8 main" | sudo tee --append /etc/apt/sources.list
    sudo apt-get update
    sudo apt-get -y install \
      cmake \
      cmake-data \
      ninja-build \
      clang-3.8 \
      clang-format-3.8 \
      gcc \
      g++-multilib \
      libc6-dev \
      libc6-dev-i386
    ;;
  *)
    echo "ERROR: Unsupported OS"
    ;;
esac
