/** \mainpage µTeX
 *
 * A dynamic, cross-platform, and embeddable LaTeX rendering library
 *
 * <table class="infotable" style="border-spacing: 0 0.25rem;">
 *   <tr>
 *     <td><em>Version</em></td>
 *     <td>1.0.0</td>
 *   </tr>
 *   <tr>
 *     <td><em>License</em></td>
 *     <td>MIT</td>
 *   </tr>
 *   <tr>
 *     <td><em>Authors</em></td>
 *     <td>NanoMichael &lt;artiano@hotmail.com&gt;</td>
 *   </tr>
 *   <tr>
 *     <td><em>Forge</em></td>
 *     <td><a href="https://github.com/NanoMichael/MicroTeX">https://github.com/NanoMichael/MicroTeX</a></td>
 *   </tr>
 * </table>
 *
 * \section introduction Introduction
 *
 * \f$\MicroTeX\f$ is a versitile library, capable of very efficiently
 * rendering most \f$\LaTeX\f$ equations on any surface that provides
 * postscript-esque vector draw instructions. If you are interested in
 * implementing \f$\MicroTeX\f$ for such a surface, that doesn't have
 * support yet, read <a href="TODO">this document</a>.
 *
 * Currently, \f$\MicroTeX\f$ supports the following libraries and
 * frameworks:
 * <ul>
 *   <li>Cairo</li>
 *   <li>QT</li>
 *   <li>win32</li>
 *   <li>WASM</li>
 *   <li>GTK</li>
 * </ul>
 *
 * \section install_sec Installation
 *
 * \subsection prebuilt Install µTeX using precompiled packages
 *
 * \subsection compile Compile µTeX from source
 *
 * To build \f$\MicroTeX\f$ yourself, please ensure that all of the
 * following dependencies are installed on your operating system and
 * accesible by pkg-config.
 *
 * <table>
 *   <tr>
 *     <th>Library</th>
 *     <th>pkg-config</th>
 *     <th>Website</th>
 *   </tr>
 *   <tr>
 *     <td colspan="3">For the <b>Cairo</b> platform:</td>
 *   </tr>
 *   <tr>
 *     <td>libfontconfig</td>
 *     <td><tt>fontconfig</tt></td>
 *     <td><a href="https://www.freedesktop.org/wiki/Software/fontconfig/">freedesktop.org/wiki/Software/fontconfig/</a></td>
 *   </tr>
 *   <tr>
 *     <td>Cairo</td>
 *     <td><tt>cairo</tt></td>
 *     <td><a href="https://cairographics.org/">cairographics.org</a></td>
 *   </tr>
 *   <tr>
 *     <td>Cairo FreeType</td>
 *     <td><tt>cairo-ft</tt></td>
 *     <td><a href="https://cairographics.org/">cairographics.org</a></td>
 *   </tr>
 *   <tr>
 *     <td>Pango</td>
 *     <td><tt>pango</tt></td>
 *     <td><a href="https://pango.gnome.org/">pango.gnome.org</a></td>
 *   </tr>
 *   <tr>
 *     <td>Pango Cairo</td>
 *     <td><tt>pangocairo</tt></td>
 *     <td><a href="https://pango.gnome.org/">pango.gnome.org</a></td>
 *   </tr>
 *   <tr>
 *     <td colspan="3">For the <b>QT</b> platform:</td>
 *   </tr>
 *   <tr>
 *     <td>QT5 Gui</td>
 *     <td><tt>qt5</tt></td>
 *     <td><a href="https://www.qt.io/">qt.io</a><br><a href="https://kde.org/community/whatiskde/kdefreeqtfoundation/">kde.org/community/whatiskde/kdefreeqtfoundation/</a></td>
 *   </tr>
 *   <tr>
 *     <td colspan="3">For the <b>win32</b> platform:</td>
 *   </tr>
 *   <tr>
 *     <td>GDI+</td>
 *     <td><tt>libgdiplus</tt></td>
 *     <td><a href="https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-gdi-start">docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-gdi-start</a><br><a href="https://github.com/mono/libgdiplus">github.com/mono/libgdiplus</a></td>
 *   </tr>
 *   <tr>
 *     <td colspan="3">For the <b>WASM</b> platform:</td>
 *   </tr>
 *   <tr>
 *     <td>Emscripten</td>
 *     <td><tt>n/a</tt></td>
 *     <td><a href="https://emscripten.org/">emscripten.org</a></td>
 *   </tr>
 *   <tr>
 *     <td colspan="3">For the <b>GTK4</b> widget (also requires the dependencies listed under Cairo):</td>
 *   </tr>
 *   <tr>
 *     <td>GLib</td>
 *     <td><tt>glib-2.0</tt></td>
 *     <td><a href="https://wiki.gnome.org/Projects/GLib">wiki.gnome.org/Projects/GLib</a></td>
 *   </tr>
 *   <tr>
 *     <td>GTK 4</td>
 *     <td><tt>gtk4</tt></td>
 *     <td><a href="https://www.gtk.org/">gtk.org</a></td>
 *   </tr>
 *   <tr>
 *     <td>Cairo SVG</td>
 *     <td><tt>cairo-svg</tt></td>
 *     <td><a href="https://cairographics.org/">cairographics.org</a></td>
 *   </tr>
 * </table>
 * <em>Please note, that the demo applications for a corresponding library might have additional dependencies</em>
 * 
 * \subsubsection meson Build µTeX using Meson
 * If you are able to use meson, please look at the <tt>meson_options.txt</tt>
 * in the root directory of \f$\MicroTeX\f$ and figure out what
 * platforms you wish to use. Then invoke:
 * @code
 *   meson _build -D<option>=<true/false>
 *   ninja -C _build
 *   ninja -C _build install
 * @endcode
 *
 * \subsubsection cmake Build µTeX using CMake
 * Alternativly, you can also build the project using cmake. Before
 * configuring, take a look at the <tt>CMakeLists.txt</tt> in the root
 * directory, and figure out what options you need to set in order to
 * build the platforms you require. Then invoke:
 * @code
 *   mkdir -p _cbuild && cd _cbuild
 *   cmake .. -D<option>=<ON/OFF>
 *   make -j<threads>
 *   sudo make install
 * @endcode
 *
 * \section Usage
 * Before rendering equations, you need to initialize \f$\MicroTeX\f$
 * by calling microtex::MicroTeX::init(). Assuming you have built with
 * fontsense enabled, you have two different init methods available.
 * Fontsense allows you not to worry about shipping your own fonts and
 * instead just lets MicroTeX look if any of it's fonts are installed
 * on the operating system and uses those (but note that the method
 * will throw should no font be found).
 * Alternativly, you can initialize with FontMeta, and just provide
 * paths to where the fonts are located. For more information about
 * the clm font format and fonts in general see \ref font_doc.
 * 
 * After initializing, you'll need to register the platform you intend
 * to use. This can be done by calling microtex::PlatformFactory::registerFactory()
 * with a unique identifier for the specific platform and an unique
 * pointer to the constructor of the platform-specific PlatformFactory
 * (see the inheritance diagramm on microtex::PlatformFactory) and look
 * into the platform-specific documentation for a full example for a
 * specific platform.
 *
 * You can now start parsing \f$\LaTeX\f$ equations, by calling
 * microtex::MicroTeX::parse(). You receive a microtex::Render, which
 * provides the microtex::Render::draw() method, which can be used
 * in combination with a platform-specific microtex::Graphics2D context
 * to render the equation onto your surface.
 * 
 * 
 * \section platform_docs Platform-specific Documentation
 *
 * \subsection platform_cairo Cairo
 * Cairo is a 2D graphics library with support for multiple output
 * devices. Currently supported output targets include the X Window
 * System (via both Xlib and XCB), Quartz, Win32, image buffers,
 * PostScript, PDF, and SVG file output. Experimental backends include
 * OpenGL, BeOS, OS/2, and DirectFB.
 *
 * Cairo is designed to produce consistent output on all output media
 * while taking advantage of display hardware acceleration when
 * available (eg. through the X Render Extension).
 *
 * It is most commonly known for it's usage by the GTK Toolkit, which
 * used to exclusivly render onto cairo surfaces. But even after the
 * move to Gsk, Cairo is still an important part of the GTK rendering
 * stack.
 *
 * To see how to use \f$\MicroTeX\f$ with Cairo see \ref cairo_doc.
 *
 * \subsection platform_qt Qt
 * Qt is a cross-platform framework for implementing graphical user
 * interfaces on various operating systems.
 *
 * It is comercially developed by the QT company, while the KDE project
 * ensures the free software nature of the library.
 *
 * To see how to use \f$\MicroTeX\f$ with QT see <a href="TODO">this page</a>.
 *
 * \subsection platform_win32 Win32
 * Microsoft Windows's native user interface stacks allows drawing of
 * postscript-like calls using its GDI+ library.
 *
 * To see how to use \f$\MicroTeX\f$ with Win32/GDI+ see <a href="TODO">this page</a>.
 *
 * \subsection platform_wasm WebAssembly
 * \f$\MicroTeX\f$ is able to render \f$\LaTeX\f$ equations onto HTML
 * canvases and generate scalable vector graphics.
 *
 * You can try it using the <a href="https://nanomichael.github.io/MicroTeX/">\f$\MicroTeX\f$ WASM demo</a>.
 *
 * \subsection platform_gtk GTK4
 *
 * For GTK4, \f$\MicroTeX\f$ provides a stand-alone \f$\LaTeX\f$ widget
 * that can be used in any programming language making use of GObject
 * introspection.
 *
 * The documentation for the widget is <a href="../MicroTex-1/index.html">here</a>.
 */
