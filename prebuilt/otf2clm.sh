#!/usr/bin/env sh

usage="
Generate clm data from OTF font file.

Usage: $(basename "$0") <otf/font/file> <true|false> <file/to/save/clm>

Required arguments:
    1: OTF font file path
    2: true or false to tell the program whether the font is a math font
    3: true or false to tell the program whether parse glyph's path
    4: target file to save the clm data
"

if [ $# -ne 4 ]
then
  echo "$usage"
  exit 1
fi

fontforge -lang=py -script otf2clm.py "$1" "$2" "$3" "$4"
echo "Generated clm data was saved into file: $4"
