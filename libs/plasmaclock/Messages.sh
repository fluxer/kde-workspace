#!/bin/bash
$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT *.cpp -o $podir/libplasmaclock.pot
rm -f rc.cpp
