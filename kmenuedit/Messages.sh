#!/bin/bash
$EXTRACTRC *.rc >> rc.cpp
$XGETTEXT *.cpp -o $podir/kmenuedit.pot
