#!/bin/bash
$EXTRACTRC `find . -name \*.ui` >> rc.cpp || exit 11
$XGETTEXT `find . -name \*.cpp` -o $podir/kwin_effects.pot
rm -f rc.cpp
