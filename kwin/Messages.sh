#!/bin/bash
$EXTRACTRC *.kcfg >> rc.cpp
$XGETTEXT *.h *.cpp killer/*.cpp tabbox/*.cpp -o $podir/kwin.pot
