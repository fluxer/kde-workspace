#!/bin/bash
$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT *.cpp -o $podir/soliduiserver.pot
