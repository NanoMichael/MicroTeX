const std = @import("std");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    // TODO: results in undefined behaviour because debug build includes
    // `-fsanitize=undefined`, which triggers an illegal instruction in
    // `ByteSeq.put`
    const optimize: std.builtin.OptimizeMode = .ReleaseFast;

    const glyph_render_type = b.option(
        []const u8,
        "GLYPH_RENDER_TYPE",
        "0: use path and typeface both (default). 1: use path only. 2: use typeface only.",
    ) orelse "0";

    const conf_header = b.addConfigHeader(
        .{
            .include_path = "microtexconfig.h",
            .style = .{ .cmake = b.path("lib/microtexconfig.h.in") },
        },
        .{
            .PROJECT_VERSION_MAJOR = 1,
            .PROJECT_VERSION_MINOR = 0,
            .PROJECT_VERSION_PATCH = 0,
            .HAVE_AUTO_FONT_FIND = true,
        },
    );

    const install_config = b.addInstallFileWithDir(
        conf_header.getOutput(),
        .header,
        "microtexconfig.h",
    );

    const lib = b.addStaticLibrary(.{
        .name = "microtex",
        .target = target,
        .optimize = optimize,
    });

    lib.step.dependOn(&install_config.step);

    lib.linkLibCpp();

    lib.addIncludePath(b.path("lib"));
    lib.defineCMacro("GLYPH_RENDER_TYPE", glyph_render_type);
    lib.addIncludePath(b.path("zig-out/include"));
    lib.addCSourceFiles(.{
        .root = b.path("lib"),
        .files = SOURCE_FILES,
        .flags = COMPILE_FLAGS,
    });

    inline for (INCLUDE_FILES) |path| {
        lib.installHeader(b.path("lib/" ++ path), path);
    }

    // include the C wrapper
    lib.addCSourceFiles(.{
        .root = b.path("lib"),
        .files = CWRAPPER_FILES,
        .flags = COMPILE_FLAGS,
    });
    lib.defineCMacro("HAVE_CWRAPPER", null);
    inline for (CWRAPPER_INCLUDE) |path| {
        lib.installHeader(b.path("lib/" ++ path), path);
    }

    b.installArtifact(lib);

    // optionally link to the memcheck executable to ensure all symbols are defined
    const exe = b.addExecutable(.{
        .name = "memcheck",
        .target = target,
        .optimize = optimize,
    });

    exe.linkLibrary(lib);
    exe.addIncludePath(b.path("zig-out/include"));
    exe.addIncludePath(b.path("example/samples"));
    exe.addIncludePath(b.path("example/memcheck"));
    exe.addCSourceFiles(.{
        .files = &.{
            "example/samples/samples.cpp",
            "example/memcheck/mem_check_main.cpp",
        },
        .flags = COMPILE_FLAGS,
    });
    exe.addIncludePath(b.path("lib"));

    const memcheck_step = b.step("memcheck", "Build the memcheck binary.");
    memcheck_step.dependOn(&b.addInstallArtifact(exe, .{}).step);
}

const CWRAPPER_FILES = &.{
    "wrapper/callback.cpp",
    "wrapper/graphic_wrapper.cpp",
    "wrapper/cwrapper.cpp",
    "wrapper/byte_seq.cpp",
};

const CWRAPPER_INCLUDE = &.{
    "wrapper/cwrapper.h",
    "wrapper/callback.h",
};

const INCLUDE_FILES = &.{
    "microtexexport.h",
    "microtex.h",
    "utils/types.h",
    "render/render.h",
    "unimath/font_src.h",
    "unimath/font_meta.h",
    "graphic/graphic.h",
    "graphic/font_style.h",
    "graphic/graphic_basic.h",
};

const COMPILE_FLAGS = &.{
    "-std=c++20",
    // "-lstdc++fs",
};

const SOURCE_FILES = &.{
    // atom folder
    "atom/atom.cpp",
    "atom/atom_basic.cpp",
    "atom/atom_char.cpp",
    "atom/atom_misc.cpp",
    "atom/atom_matrix.cpp",
    "atom/atom_row.cpp",
    "atom/atom_space.cpp",
    "atom/atom_stack.cpp",
    "atom/atom_accent.cpp",
    "atom/atom_scripts.cpp",
    "atom/atom_vrow.cpp",
    "atom/atom_operator.cpp",
    "atom/atom_zstack.cpp",
    "atom/atom_sideset.cpp",
    "atom/atom_font.cpp",
    "atom/atom_delim.cpp",
    "atom/atom_root.cpp",
    "atom/atom_frac.cpp",
    "atom/atom_fence.cpp",
    "atom/atom_box.cpp",
    "atom/atom_text.cpp",
    "atom/colors_def.cpp",
    // box folder
    "box/box.cpp",
    "box/box_factory.cpp",
    "box/box_group.cpp",
    "box/box_single.cpp",
    // core folder
    "core/split.cpp",
    "core/formula.cpp",
    "core/formula_def.cpp",
    "core/glue.cpp",
    "core/localized_num.cpp",
    "core/parser.cpp",
    "core/debug_config.cpp",
    // macro folder
    "macro/macro.cpp",
    "macro/macro_def.cpp",
    "macro/macro_misc.cpp",
    "macro/macro_scripts.cpp",
    "macro/macro_accent.cpp",
    "macro/macro_colors.cpp",
    "macro/macro_space.cpp",
    "macro/macro_delims.cpp",
    "macro/macro_frac.cpp",
    "macro/macro_styles.cpp",
    "macro/macro_fonts.cpp",
    // env folder
    "env/env.cpp",
    "env/units.cpp",
    // utils folder
    "utils/string_utils.cpp",
    "utils/utf.cpp",
    "utils/utils.cpp",
    // otf folder
    "otf/clm.cpp",
    "otf/fontsense.cpp",
    "otf/glyph.cpp",
    "otf/otf.cpp",
    "otf/path.cpp",
    // unimath folder
    "unimath/font_src.cpp",
    "unimath/math_type.cpp",
    "unimath/uni_char.cpp",
    "unimath/uni_font.cpp",
    "unimath/uni_symbol.cpp",
    // graphic folder
    "graphic/font_style.cpp",
    "graphic/graphic_basic.cpp",
    "graphic/graphic.cpp",
    // render folder
    "render/render.cpp",
    "render/builder.cpp",

    "microtex.cpp",
};
