/** \page cairo_doc Cairo Documentation
 *
 * \section cairo_opts Build options
 *
 * \subsection Meson
 * to build support for the Cairo platform using meson, you need to
 * invoke meson using <tt>-DCAIRO=true</tt> as parameter.
 *
 * If you want to build the GTK3MM demo too, you need to add the
 * <tt>-DEXAMPLE_GTKMM=true</tt> to the params too.
 *
 * \subsection CMake
 *
 * for cmake, you need to configure it, with <tt>-DCAIRO=ON</tt>
 * and optionally with <tt>-DBUILD_EXAMPLE_GTKMM=ON</tt>.
 *
 * \section cairo_usage Usage
 *
 * \subsection cairo_init Initialization and PlatformFactory setup
 * Refer to \ref index "the main page" for info about information about automatic
 * font detection. The PlatformFactory setup is cairo specific.
 * @code
 *  microtex::InitFontSenseAuto autoinit;
 *  microtex::MicroTeX::init(autoinit);
 *  microtex::PlatformFactory::registerFactory("gtk", std::make_unique<microtex::PlatformFactory_cairo>());
 *  microtex::PlatformFactory::activate("gtk");
 * @endcode
 *
 * \subsection cairo_render Rendering
 * You need to make use of microtex::Graphics2D_cairo to render
 * equations onto a cairo surface. The graphics context takes a pointer
 * to a cairo context (<tt>cairo_t</tt>). Please note that the caller
 * keeps the ownership of the cairo context.
 *
 * To receive a microtex::Render, you can either use the convinience
 * method microtex::MicroTeX::parse(), or construct it yourself using
 * microtex::RenderBuilder if you need more customization.
 *
 * @code
 *   void render_to_cairo(cairo_t* ctx, microtex::Render* tex) {
 *     microtex::Graphics2D_cairo g2(ctx);
 *     tex->draw(g2, 0, 0);
 *   }  
 * @endcode
 *
 */
