#! /bin/sh
$EXTRACTRC kdesavers/*.ui >> rc.cpp
$XGETTEXT kdesavers/*.cpp rc.cpp -o $podir/klock.pot
rm -f rc.cpp
