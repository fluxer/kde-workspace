#! /bin/sh
$EXTRACTRC kdesavers/*.ui >> rc.cpp
$XGETTEXT kdesavers/*.cpp xsavers/*.cpp  rc.cpp -o $podir/klock.pot
rm -f rc.cpp
