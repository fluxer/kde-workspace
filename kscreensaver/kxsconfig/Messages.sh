#! /bin/sh
$XGETTEXT -x hacks.pot *.cpp -o $podir/kxsconfig.pot
cat hacks.pot >> $podir/kxsconfig.pot

