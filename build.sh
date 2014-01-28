#!/bin/sh
set -x

# Add any build steps you need here

#uname -a
#lsb_release -a
#free -m
#top -b -n1
#ps auxf

#sudo aptitude install libcmph-dev libcmph0
unxz -k test/data/*.xz
pwd
ls
g++ -static -std=c++0x -Wall -g3 -o level0 level0.cc -L. -lcmph
