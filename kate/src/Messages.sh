#! /usr/bin/env bash
(cd data && $PREPARETIPS > ../tips.cpp)
$EXTRACTRC `find . -name \*.rc -o -name \*.ui` >> rc.cpp || exit 11
$XGETTEXT `find . -name "*.cpp" -o -name "*.h"` -o $podir/kate.pot
rm -f tips.cpp
