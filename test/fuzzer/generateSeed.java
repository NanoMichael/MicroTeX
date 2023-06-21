//usr/bin/env java "$0" "$@"; exit $?

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.stream.Collectors;

public class generateSeed {
	public static void main(String[] args) throws java.io.IOException {
		Arrays.stream(
			Files.readString(Path.of("../../res/SAMPLES.tex"))
				.split("\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n")
		).collect(
			Collectors.toMap(
				s -> s.lines().findFirst().get().replaceFirst("%", ""),
				s -> s
			)
		).forEach(
			(k,v) -> {
				try {
					Files.writeString(Path.of("seeds", k + ".tex"), v);
				} catch (java.io.IOException e) {
					throw new RuntimeException(e);
				}
			}
		);
	}
}
