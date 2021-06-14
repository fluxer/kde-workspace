#!/bin/sh
#
# Note: This file is taken, and modified, from gucharmap - svn revision 1169
#
# usage: ./download-unicode-files.sh VERSION
# downloads following files from unicode.org to unicode/:
#  - UnicodeData.txt
#  - Unihan.zip
#  - NamesList.txt
#  - Blocks.txt
#  - Scripts.txt
#

set -e

FILES='UnicodeData.txt Unihan.zip NamesList.txt Blocks.txt Scripts.txt'

mkdir -p unicode

for x in $FILES; do
	wget "https://unicode.org/Public/$1/ucd/$x" -O "unicode/$x"
done

echo 'Done.'

