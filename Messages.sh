#! /usr/bin/env bash
$EXTRACTRC `find .  -name \*.rc` >> rc.cpp
$XGETTEXT `find . -name \*.cpp -o -name \*.h` rc.cpp -o $podir/kopete_sharemydesktop.pot
rm -f rc.cpp
