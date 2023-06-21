#!/bin/sh
MICROTEX_FONTDIR=../../res/lm-math/ AFL_SKIP_CPUFREQ=1 afl-fuzz -i seeds/ -o output/ -x microtex.dict -- ../../_fuzzi/test/fuzzer/microtex-fuzzme
