#!/bin/bash
### TODO: why do we need 2 POT files for a single directory?
$XGETTEXT kaccess.cpp main.cpp -o $podir/kaccess.pot
$XGETTEXT kcmaccess.cpp -o $podir/kcmaccess.pot
