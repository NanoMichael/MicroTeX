#!/bin/bash
# vim: set noet ts=2 sw=2:
set -e

project_root="../../"
build_root="./_fuzzi"

seeds_dir="./seeds/"
fuzz_out="./output/"

function print_help {
	echo "Usage: fuzz.sh [--project-root <root>] [--build-root <intrumented build root>] [--seeds-dir <dir>] [--fuzzer-output-dir <dir>] <setup | [--no-cpu-check]>"
}

while getopts ":h:p:b:s:o:-:" opt; do
	case $opt in
		h)
			print_help
			exit 0
			;;
		p)
			project_root="$OPTARG"
			;;
		b)
			build_root="$OPTARG"
			;;
		s)
			seeds_dir="$OPTARG"
			;;
		o)
			fuzz_out="$OPTARG"
			;;
		-)
			case "${OPTARG}" in
				help)
					print_help
					exit 0
					;;
				project-root)
					project_root="${!OPTIND}"
					OPTIND=$((OPTIND + 1))
					;;
				build-root)
					build_root="${!OPTIND}"
					OPTIND=$((OPTIND + 1))
					;;
				seeds-dir)
					seeds_dir="${!OPTIND}"
					OPTIND=$((OPTIND + 1))
					;;
				fuzzer-output-dir)
					fuzz_out="${!OPTIND}"
					OPTIND=$((OPTIND + 1))
					;;
				no-cpu-check)
					export AFL_SKIP_CPUFREQ=1
					;;
				*)
					echo "Invalid option: --${OPTARG}" >&2
					exit 1
					;;
			esac
			;;
		\?)
			echo "Invalid option: -$OPTARG" >&2
			exit 1
			;;
	esac
done

shift $((OPTIND - 1))

dir="$project_root/test/fuzzer/"

case $1 in
	setup)
		echo "Running setup..."
		if [ -d "$build_root" ]; then
			echo "Build root \"$build_root\" already exists. Delete?"
			# Credit to Tiago Lopo and John Kugelman on SO (https://stackoverflow.com/a/29436423)
			# Licensed under CC-BY-SA-3.0
			while true; do
				read -p "  [y/N]:" inp
				case "$inp" in
					[Yy*])
						rm -rf -- "$build_root"
						break
						;;
					*)
						echo "Aborting setup. Have a great day!"
						exit 1
						;;
				esac
			done
		fi
		meson setup "$build_root" "$project_root" --native-file "$dir/afl.native"
		mkdir -p "$seeds_dir" "$fuzz_out"
		"$dir/generateSeed.java" "$project_root/res/SAMPLES.tex" "$seeds_dir"
		exit 0
		;;
	min)
		echo "Minimizing all crashes..."
		CRASHDIR="$fuzz_out/default/crashes/"
		if [ ! -d "$CRASHDIR" ]; then
			echo "Did not found crash directory at expected location: $CRASHDIR, please run the fuzzer first."
			exit 1
		fi
		MINOUT="$fuzz_out/default/crashes-min/"
		mkdir -p "$MINOUT"
		for crash in "$CRASHDIR"/id*; do
			MICROTEX_FONTDIR="$project_root/res/lm-math/" afl-tmin -i "$crash" -o "$MINOUT"/`basename "$crash"` -- "$build_root/test/fuzzer/microtex-fuzzme"
		done
		exit 0
		;;
	dbg)
		if [ -z "$2" ]; then
			echo "You need to specify the specific crash. ./fuzz.sh [options] dbg <path/to/crashfile>"
			exit 1
		fi
		ninja -C "$build_root"
		MICROTEX_FONTDIR="$project_root/res/lm-math/" gdb  "$build_root/test/fuzzer/microtex-fuzzme" -ex "run <'$2'"
		exit 0
		;;
	"")
		;;
	*)
		echo "Invalid argument: $1."
		print_help
		exit 1
		;;
esac

ninja -C "$build_root"
mkdir -p "$fuzz_out"
MICROTEX_FONTDIR="$project_root/res/lm-math/" afl-fuzz -i "$seeds_dir" -o "$fuzz_out" -x "$dir/microtex.dict" -- "$build_root/test/fuzzer/microtex-fuzzme"
