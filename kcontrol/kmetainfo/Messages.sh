#!/bin/bash
$EXTRACTRC *.ui >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/kcm_kmetainfoconfig.pot
rm -f rc.cpp
