#!/usr/bin/env sh

echo "generate firamath..."
./otf2clm.sh --batch ../res/firamath false ../res/firamath
./otf2clm.sh --batch ../res/firamath true ../res/firamath

echo "generate lm-math..."
./otf2clm.sh --batch ../res/lm-math false ../res/lm-math
./otf2clm.sh --batch ../res/lm-math true ../res/lm-math

echo "generate tex-gyre..."
./otf2clm.sh --batch ../res/tex-gyre false ../res/tex-gyre
./otf2clm.sh --batch ../res/tex-gyre true ../res/tex-gyre

echo "generate xits..."
./otf2clm.sh --batch ../res/xits false ../res/xits
./otf2clm.sh --batch ../res/xits true ../res/xits
