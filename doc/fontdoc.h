/** \page font_doc CLM Font Format
 *
 * \section font_preamble Preamble
 * Fonts are a core concept of \f$\MicroTeX\f$, as some kind of Font
 * is definitively required to set an equation.
 *
 * The CLM Font format is made exactly to fulfill this need of font
 * information for \f$\MicroTeX\f$. It is a fairly simple file format,
 * containing  font info, glyph metrics, math tables, and other
 * significant information to layout math formulas, with this data
 * simply being read linearly. It does not contain any kind of error
 * checking or similar.
 *
 * Because \f$\MicroTeX\f$ might be used on platforms without working
 * font renderer/provider, and it is capable of drawing paths anyways,
 * there are two different kind of font files.
 * <ul>
 *   <li><b>Font without glyph paths</b>
 *     <p>
 *       Fonts without glyph paths have the extension `.clm1` (and byte
 *       `0x01` at offset `0x5`). They will NOT contain any glyphs paths
 *       and it will instead use the platforms provided methods to load
 *       the fontFile provided by the microtex::FontSrc.
 *     </p>
 *     <p>
 *       In order to render the Glyph it will simply call the platform
 *       provided microtex::Graphics2D::drawGlyph(), and the platform
 *       will take of rendering the specified glyph from the opentype
 *       font file it has received earlier.
 *     </p>
 *   </li>
 *   <li><b>Font with glyph paths</b>
 *     <p>
 *       Fonts with glyph paths have the extension `.clm2` (and byte
 *       `0x02` at offset `0x5`). They will contain all the glyphs paths
 *       (that obviously means the "clm" data file will be a lot bigger)
 *       and it will use the PostScript-esque vector drawing calls from
 *       the platform to render the Glyph data also contained in the
 *       file.
 *     </p>
 *     <p>
 *       Using a font with included glyph path has the benefit, that
 *       you don't need the font file anymore and you can use them on
 *       before-mentioned platforms that do not/have broken support
 *       rendering opentype fonts.
 *       However using a font with glyph path may have legal
 *       implications/issues, depending on how the font is licensed
 *       (e.g. any CC license with ND won't work).
 *     </p>
 *   </li>
 * </ul>
 *
 * \section font_usage Using Fonts
 * \f$\MicroTeX\f$ can be compiled to just use one specific render type.
 * Depending on your usecase, you might want to add one of the following
 * build arguments:
 *
 * <table>
 *   <tr>
 *     <th/>
 *     <th>Meson</th>
 *     <th>CMake</th>
 *   </tr>
 *   <tr>
 *     <td>Support for both glyph path and typeface render types (default)</td>
 *     <td>`-DGLYPH_RENDER_TYPE=both`</td>
 *     <td>`-DGLYPH_RENDER_TYPE=0`</td>
 *   </tr>
 *   <tr>
 *     <td>Only include support for rendering glyphs via the typeface render type</td>
 *     <td>`-DGLYPH_RENDER_TYPE=typeface`</td>
 *     <td>`-DGLYPH_RENDER_TYPE=2`</td>
 *   </tr>
 *   <tr>
 *     <td>Only include support for rendering glyphs via the path render type</td>
 *     <td>`-DGLYPH_RENDER_TYPE=path`</td>
 *     <td>`-DGLYPH_RENDER_TYPE=1`</td>
 *   </tr>
 * </table>
 *
 * \subsection font_usage_manual Manually
 * In order to add fonts to \f$\MicroTeX\f$, you need to provide a
 * microtex::FontSrc to microtex::MicroTeX::init() (note that the
 * provided font needs to be a mathfont) or microtex::MicroTeX::addFont().
 *
 * If you intend to manually specify Fonts, you can either use
 * microtex::FontSrcFile or microtex::FontSrcData. Both constructors of
 * each of the classes have an optional second parameter. If you intend
 * to you the glyph path render type you need to leave this empty.
 * Otherwise, you'll want to specify the path to the opentype font file
 * that the platform load and draw glyphs from (typeface path).
 *
 * @code
 *   const FontSrcFile math = FontSrcFile {"path/to/latinmodern-math.clm1", "path/to/latinmodern-math.otf"};
 *   microtex::MicroTeX::init(&math);
 * @endcode
 *
 * \subsection font_usage_fontsense FontSense (Automatic)
 * If you don't need/want to specify any specific fonts, you can also
 * use FontSense to let \f$\MicroTeX\f$ automatically scan the system
 * for available fonts.
 *
 * To use FontSense simply provide microtex::MicroTeX::init() with
 * microtex::InitFontSenseAuto:
 * @code
 *   microtex::InitFontSenseAuto autoinit;
 *   microtex::MicroTeX::init(autoinit);
 * @endcode
 *
 * microtex::MicroTeX::init will throw microtex::ex_invalid_param,
 * should no font be available on the system. You may want to catch
 * this, you may not... (Depending on the kind of userbase that will be
 * using your application)
 *
 * But if \f$\MicroTeX\f$ was installed via a precompiled package,
 * at least one mathfont should always be available.
 *
 * \subsubsection font_usage_fontsense_dirs FontSense Directories
 * FontSense will look in the following directories for files with
 * `.clm1` and `.clm2` extensions (note that .clm1 files need a .otf
 * file with the same filestem (name before the last dot) in the same
 * directory as itself in order to be recognized):
 *
 * <ul>
 *   <li>`$MICROTEX_FONTDIR`</li>
 *   <li>`$XDG_DATA_HOME/microtex`</li>
 *   <li>(each dir in `$XDG_DATA_DIRS`)/`microtex`</li>
 *   <li>`$HOME/.local/share/microtex` (not on win32)</li>
 *   <li>`/usr/local/share/microtex` (not on win32)</li>
 *   <li>`/usr/share/microtex` (not on win32)</li>
 *   <li>`<dir of executable>/share/microtex` (only on win32)</li>
 * </ul>
 *
 * Note that MicroTeX will need to be compiled with `-DHAVE_AUTO_FONT_FIND=true` (Meson) / `-DHAVE_AUTO_FONT_FIND=ON` (CMake). But this being toggled on is their default behaviour.
 *
 * \section font_generation Generating new CLM fonts
 * \f$\MicroTeX\f$ provides an `otf2clm` script that automatically
 * generates the correct CLM font(s) from any opentype font.
 *
 * When \f$\MicroTeX\f$ is available as package from your distribution,
 * the script should be part of the `microtex-tools` subpackage
 * (assuming your distro does split microtex into subpackages), and
 * ready to execute by invoking `microtex-otf2clm`. Alternatively, it
 * is in-tree located in `prebuilt/otf2clm.sh` (note you need to have
 * fontforge version 20200314 or later installed and on your PATH, and
 * that cwd needs to be `prebuilt/` when invoking).
 *
 * \subsection font_generation_usage Usage
 *
 * \subsubsection font_generation_usage_single Convert single font
 * ```sh
 * <otf2clm> --single \\
 *   <path/to/OTF-font.otf> \\
 *   <include glyph path in clm file: true | false> \\
 *   <output_directory> \\
 *   [comma seperated font-style list]
 * ```
 *
 * Possible font styles are:
 * <ul>
 *   <li>`&nbsp;&nbsp;rm:` Roman / Serif</li>
 *   <li>`&nbsp;&nbsp;bf:` Bold</li>
 *   <li>`&nbsp;&nbsp;it:` Italic</li>
 *   <li>`     &nbsp;cal:` Calligraphic</li>
 *   <li>`          frak:` Fraktur</li>
 *   <li>`&nbsp;&nbsp;bb:` Blackboard</li>
 *   <li>`&nbsp;&nbsp;sf:` Sans Serif</li>
 *   <li>`&nbsp;&nbsp;tt:` Typewriter / Monospace</li>
 * </ul>
 *
 * Specifying a font-style list is optional. If you do not provide
 * anything or provide an empty string, `otf2clm` will automatically
 * attempt to determine a fitting font-style.<br>
 * The automatic detection is however only capable if detecting the
 * following  styles: `rm, bf, it, tt`.
 *
 * \subsubsection font_generation_usage_batch Convert multiple fonts
 * ```sh
 * <otf2clm> --batch \\
 *   <path/to/dir_with_fonts/> \\
 *   <include glyph path in clm file: true | false> \\
 *   <output_directory>
 * ```
 *
 * It is not possible to define font styles in batch mode. otf2clm will
 * set the font-style of each specific font purely based on the result
 * of its auto-detection (same limits as in \ref font_generation_usage_single).
 *
 * \section font_format Font Format
 * \subsection font_format_meta Metadata
 * <table>
 *   <tr>
 *     <th>Name</th>
 *     <th>Type</th>
 *     <th>Size</th>
 *     <th>Description</td>
 *   </tr>
 *   <tr>
 *     <td>Magic bytes</td>
 *     <td>`char[3]`</td>
 *     <td>3 bytes</td>
 *     <td>Expected data: clm</td>
 *   </tr>
 *   <tr>
 *     <td>Version</td>
 *     <td>`unsigned short`</td>
 *     <td>2 bytes</td>
 *     <td>The current version of the file format (i.e. currently `5`), \f$\MicroTeX\f$ will reject anything but the latest version</td>
 *   </tr>
 *   <tr>
 *     <td>Glyph Type</td>
 *     <td>`unsigned char`</td>
 *     <td>1 byte</td>
 *     <td>Contains `0x02` if the font file contains glyph paths, `0x01` otherwise</td>
 *   </tr>
 *   <tr>
 *     <td>Font name</td>
 *     <td>`char*`</td>
 *     <td>use `strlen()`</td>
 *     <td>Contains the name of the font. Read this string until you hit a `NULL` byte.</td>
 *   </tr>
 *   <tr>
 *     <td>Family name</td>
 *     <td>`char*`</td>
 *     <td>use `strlen()`</td>
 *     <td>Contains the family name of the font. Read this string until you hit a `NULL` byte.</td>
 *   </tr>
 *   <tr>
 *     <td>Is math font</td>
 *     <td>`boolean`</td>
 *     <td>1 byte</td>
 *     <td>`0x01` if the font contains math constants, `0x00` otherwise</td>
 *   </tr>
 *   <tr>
 *     <td>Style</td>
 *     <td>`unsigned short`</td>
 *     <td>2 bytes</td>
 *     <td>Bitset representing the style of the font.
 *      <p>Bits are:
 *        <ul>
 *          <li>`&nbsp;&nbsp;rm: 0b00000001` (Roman / Serif)</li>
 *          <li>`&nbsp;&nbsp;bf: 0b00000010` (Bold)</li>
 *          <li>`&nbsp;&nbsp;it: 0b00000100` (Italic)</li>
 *          <li>`     &nbsp;cal: 0b00001000` (Calligraphic)</li>
 *          <li>`          frak: 0b00010000` (Fraktur)</li>
 *          <li>`&nbsp;&nbsp;bb: 0b00100000` (Blackboard)</li>
 *          <li>`&nbsp;&nbsp;sf: 0b01000000` (Sans Serif)</li>
 *          <li>`&nbsp;&nbsp;tt: 0b10000000` (Typewriter / Monospace)</li>
 *        </ul>
 *      </p>
 *     </td>
 *   </tr>
 *   <tr>
 *     <td>Em (Size)</td>
 *     <td>`unsigned short`</td>
 *     <td>2 bytes</td>
 *     <td>The em size of the font</td>
 *   </tr>
 *   <tr>
 *     <td>X-Height</td>
 *     <td>`unsigned short`</td>
 *     <td>2 bytes</td>
 *     <td>The height of lower case letters such as "x". A negative number indicates the value could not be computed (the font might have no lower case letters because it was upper case only, or didn’t include glyphs for a script with lower case letters).</td>
 *   </tr>
 *   <tr>
 *     <td>Ascent</td>
 *     <td>`unsigned short`</td>
 *     <td>2 bytes</td>
 *     <td>The font's ascent</td>
 *   </tr>
 *   <tr>
 *     <td>Descent</td>
 *     <td>`unsigned short`</td>
 *     <td>2 bytes</td>
 *     <td>The font's descent</td>
 *   </tr>
 * </table>
 * \subsection font_format_unicode  Unicode Glyph Map
 * <table>
 *   <tr>
 *     <th>Name</th>
 *     <th>Type</th>
 *     <th>Size</th>
 *     <th>Description</td>
 *   </tr>
 *   <tr>
 *     <td>Number of Glyphs</td>
 *     <td>`unsigned short`</td>
 *     <td>2 bytes</td>
 *     <td>The number of items in the glyph map. Required to read the next row</td>
 *   </tr>
 *   <tr>
 *     <td>Unicode Glyph</td>
 *     <td>
 *       `typedef struct { unsigned int codepoint; unsigned short glyph_id; } UniMapItem;`<br>
 *       `UniMapItem[Number of Glyphs]`
 *     </td>
 *     <td>
 *       6*(Number of Glyphs) bytes
 *     </td>
 *     <td>This is an array that should be parsed into some kind of map structure</td>
 *   </tr>
 * </table>
 * \subsection font_format_kerning  Kerning Class
 * // TODO
 * \subsection font_format_ligatures  Ligatures
 * // TODO
 * \subsection font_format_math  Math Constants
 * // TODO
 * \subsection font_format_glyphs  Glyphs
 * // TODO
 *
 */