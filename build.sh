#!/bin/sh
set -x

# Add any build steps you need here

#sudo aptitude install libcmph-dev libcmph0
unxz -k test/data/*.xz
pwd
ls
xxd -i test/data/lower.txt > test/data/lower.h
xxd -i test/data/packed > test/data/packed.h
g++ -flto -O3 -static -std=c++0x -Wall -o level0 level0.cc -L. -lcmph
#g++ -g3 -static -std=c++0x -Wall -o level0 level0.cc -L. -lcmph
test/harness
