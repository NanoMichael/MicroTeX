//usr/bin/env java "$0" "$@"; exit $?

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.stream.Collectors;

public class generateSeed {
	public static void main(String[] args) throws java.io.IOException {
		if (args.length < 1 || args.length > 2) {
			System.err.println("Usage: generateSeed.java <path/to/SAMPLES.tex> [seeds_output_dir]");
			System.exit(1);
		}

		Arrays.stream(
			Files.readString(Path.of(args[0]))
				.split("\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n")
		).collect(
			Collectors.toMap(
				s -> s.lines().findFirst().get().replaceFirst("%", ""),
				s -> s
			)
		).forEach(
			(k,v) -> {
				try {
					Files.writeString(Path.of(args.length == 2 ? args[1] : "seeds", k + ".tex"), v);
				} catch (java.io.IOException e) {
					throw new RuntimeException(e);
				}
			}
		);
	}
}
