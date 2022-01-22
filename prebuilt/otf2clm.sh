#!/usr/bin/env sh

if [ $# -ne 4 ]
then
  fontforge -lang=py -script otf2clm.py
  exit 1
fi

fontforge -lang=py -script otf2clm.py "$1" "$2" "$3" "$4"
