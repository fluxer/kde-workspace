#!/bin/bash
$EXTRACTRC kcm*.ui >> rc.cpp
$XGETTEXT rc.cpp *.cpp -o $podir/kcmkeyboard.pot
rm -f rc.cpp 
