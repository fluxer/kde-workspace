#!/bin/bash
$XGETTEXT `find . -name "*.cc" -o -name "*.cpp" -o -name "*.h"` -o $podir/kio_settings.pot
