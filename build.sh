#! /bin/bash

CC=clang++
CXXSTD=--std=c++20
CWARN=-Wall -Wextra -Wpedantic -Werror
INCLUDES=-I/home/$USER/.local/lib/glfw/include
LIBS="-lGL -L/home/$USER/.local/lib/glfw/lib -lglfw3"
SRC="../src/*.cpp"

cd /home/$USER/repo/amelie/build
set -xe
$CC $CXXSTD $CWARN $INCLUDES -o amelie $SRC $LIBS  && ../build/amelie
