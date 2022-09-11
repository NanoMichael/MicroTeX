/** \page qt_doc Qt Documentation
 *
 * \section qt_opts Build options
 *
 * \subsection Meson
 * to build support for the Qt platform using meson, you need to
 * invoke meson using <tt>-DQT=true</tt> as parameter.
 *
 * If you want to build the Qt demo too, you need to add the
 * <tt>-DEXAMPLE_QT=true</tt> to the params too.
 *
 * \subsection CMake
 *
 * for cmake, you need to configure it, with <tt>-DQT=ON</tt>
 * and optionally with <tt>-DBUILD_EXAMPLE_QT=ON</tt>.
 *
 * \section qt_usage Usage
 *
 * \subsection qt_init Initialization and PlatformFactory setup
 * Refer to \ref index "the main page" for info about information about automatic
 * font detection. The PlatformFactory setup is qt specific.
 * @code
 *  microtex::InitFontSenseAuto autoinit;
 *  microtex::MicroTeX::init(autoinit);
 *  microtex::PlatformFactory::registerFactory("qt", std::make_unique<microtex::PlatformFactory_qt>());
 *  microtex::PlatformFactory::activate("qt");
 * @endcode
 *
 * \subsection qt_render Rendering
 * You need to make use of microtex::Graphics2D_qt to render
 * equations onto a QWidget with QPainter. Please note that the caller
 * keeps the ownership of the cairo context.
 *
 * Before use microtex::Graphics2D_qt, the microtex::Render must be initialized.
 * You can either use the convenience
 * method microtex::MicroTeX::parse(), or construct it yourself using
 * microtex::RenderBuilder if you need more customization.
 *
 * @code
 *   void MyWidget::init_render() {
 *       // _render is a field of MyWidget class
 *       // microtex::Render* _render;
 *       _render = microtex::MicroTeX::parse(args...);
 *       // or use microtex::RenderBuilder
 *   }
 *   void MyWidget::paintEvent(QPaintEvent* event) {
 *       QPainter painter(this);
 *       painter.setRenderHint(QPainter::Antialiasing, true);
 *       Graphics2D_qt g2(&painter);
 *       _render.draw(g2, 0, 0);
 *   }
 * @endcode
 *
 */
