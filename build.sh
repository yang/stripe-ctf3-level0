#!/bin/sh

# Add any build steps you need here

sudo aptitude install libcmph-dev libcmph0
unxz -k test/data/*.xz
g++ -std=c++0x -Wall -g3 -o level0 level0.cc -lcmph
