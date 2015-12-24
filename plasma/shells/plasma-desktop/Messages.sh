#!/bin/bash
$EXTRACTRC `find . -name \*.kcfg` >> rc.cpp
$EXTRACTRC `find . -name \*.ui` >> rc.cpp
$XGETTEXT *.cpp scripting/*.cpp -o $podir/plasma-desktop.pot
rm -f rc.cpp
