#include "microtexequation.h"

#include <cairo-svg.h>
#include <cairo.h>
#include <graphic_cairo.h>
#include <microtex.h>
#include <utils/exceptions.h>
#include <utils/nums.h>
namespace tex = microtex;

class MicroTeXErrorState {
public:
  tex::ex_tex exception;

  void (*ui_handler)(cairo_t*);

  gint width;
  gint height;
  gint baseline;

  MicroTeXErrorState(
    tex::ex_tex& exception,
    void (*fun)(cairo_t*),
    gint width = 128,
    gint height = 16,
    gint baseline = 0
  )
      : exception(std::move(exception)),
        ui_handler(fun),
        width(width),
        height(height),
        baseline(baseline) {}
};

/**
 * MicroTexEquation:
 *
 * `MicroTexEquation` renders a LaTeX equation using
 * [µTeX](https://github.com/NanoMichael/MicroTeX)
 *
 * A basic implementation could be as simple as
 * ```c
 * // allocate this widget
 * GtkWidget* equation = microtex_equation_new();
 * // set the width used to determine when to linebreak (0 means no breaking)
 * microtex_equation_set_width(equation, 0.f);
 * // set the LaTeX string that will be rendered
 * microtex_equation_set_equation(equation, "\\LaTeX");
 * // insert widget into parent: e.g.
 * gtk_box_append(GTK_BOX(parent), equation);
 * ```
 *
 * A far more in-depth demo application / example is available in the
 * [µTeX github repository](https://github.com/NanoMichael/MicroTeX)
 * under `example/gtk/`.
 */

typedef struct {
  gboolean shown;

  tex::Render* microtex;
  MicroTeXErrorState* error;

  gchar* equation;
  gfloat width;

  gfloat text_size;
  guint32 color;

} MicroTexEquationPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(MicroTexEquation, microtex_equation, GTK_TYPE_WIDGET)

#define MICROTEX_EQUATION_GET_PRIVATE(obj)                         \
  static_cast<MicroTexEquationPrivate*>(                           \
    microtex_equation_get_instance_private(MICROTEX_EQUATION(obj)) \
  )

enum { PROP_EQUATION = 1, PROP_WIDTH, PROP_TEXT_SIZE, PROP_COLOR, N_PROPERTIES };

static GParamSpec* obj_properties[N_PROPERTIES] = {
  NULL,
};

static void
microtex_equation_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec) {
  MicroTexEquation* self = MICROTEX_EQUATION(object);

  switch (prop_id) {
    case PROP_EQUATION: g_value_set_string(value, microtex_equation_get_equation(self)); break;
    case PROP_WIDTH: g_value_set_float(value, microtex_equation_get_width(self)); break;
    case PROP_TEXT_SIZE: g_value_set_float(value, microtex_equation_get_text_size(self)); break;
    case PROP_COLOR: g_value_set_uint(value, microtex_equation_get_color(self)); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
  }
}

static void microtex_equation_set_property(
  GObject* object,
  guint prop_id,
  const GValue* value,
  GParamSpec* pspec
) {
  MicroTexEquation* self = MICROTEX_EQUATION(object);

  switch (prop_id) {
    case PROP_EQUATION: microtex_equation_set_equation(self, g_value_get_string(value)); break;
    case PROP_WIDTH: microtex_equation_set_width(self, g_value_get_float(value)); break;
    case PROP_TEXT_SIZE: microtex_equation_set_text_size(self, g_value_get_float(value)); break;
    case PROP_COLOR: microtex_equation_set_color(self, g_value_get_uint(value)); break;
    default: G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
  }
}

static void microtex_equation_dispose(GObject* object) {
  MicroTexEquationPrivate* priv = MICROTEX_EQUATION_GET_PRIVATE(object);

  if (priv->microtex) delete priv->microtex;
  if (priv->error) delete priv->error;
  if (priv->equation) g_free(priv->equation);

  G_OBJECT_CLASS(microtex_equation_parent_class)->dispose(object);
}

static GtkSizeRequestMode microtex_equation_get_request_mode(GtkWidget* widget) {
  return GTK_SIZE_REQUEST_CONSTANT_SIZE;
}
static void microtex_equation_measure(
  GtkWidget* widget,
  GtkOrientation orientation,
  int for_size,
  int* min,
  int* nat,
  int* min_baseline,
  int* nat_baseline
) {
  MicroTexEquationPrivate* priv = MICROTEX_EQUATION_GET_PRIVATE(widget);
  if (!priv->microtex) {
    g_info("uninitialized microtex\n");
    if (!priv->error) {
      g_critical("uninitialized microtex and error state\n");
      return;
    }

    if (orientation == GTK_ORIENTATION_HORIZONTAL) {
      *min = priv->error->width;
      *nat = priv->error->width;
    }
    if (orientation == GTK_ORIENTATION_VERTICAL) {
      *min = priv->error->height;
      *nat = priv->error->height;
      *min_baseline = priv->error->baseline;
      *nat_baseline = priv->error->baseline;
    }
    return;
  }

  if (orientation == GTK_ORIENTATION_HORIZONTAL) {
    *min = priv->microtex->getWidth() + 1;
    *nat = priv->microtex->getWidth() + 1;
  }
  if (orientation == GTK_ORIENTATION_VERTICAL) {
    *min = priv->microtex->getHeight() + 1;
    *nat = priv->microtex->getHeight() + 1;
    if (priv->microtex->getBaseline() == microtex::F_MIN || std::isnan(priv->microtex->getBaseline()))
      return;
    /* *min_baseline = priv->microtex->getBaseline();
    *nat_baseline = priv->microtex->getBaseline();
    // when there is no rendered LaTeX, µTeX sets baseline to -F_MAX
    if (*min_baseline == G_MININT32)
            *min_baseline = 0;
    if (*nat_baseline == G_MININT32)
            *nat_baseline = 0;*/
    gint baseline = (gint)round(0 + *min * (1.0f - priv->microtex->getBaseline()));
    *min_baseline = baseline;
    *nat_baseline = baseline;
  }
}
static void microtex_equation_snapshot(GtkWidget* widget, GtkSnapshot* snapshot) {
  MicroTexEquationPrivate* priv = MICROTEX_EQUATION_GET_PRIVATE(widget);

  gdouble width = gtk_widget_get_allocated_width(widget),
          height = gtk_widget_get_allocated_height(widget);

  graphene_rect_t rectangle;
  graphene_rect_t* rect = graphene_rect_init(&rectangle, 0, 0, width, height);
  // do I have to free this?

  cairo_t* ctx = gtk_snapshot_append_cairo(snapshot, rect);

  if (!priv->microtex) {
    g_info("uninitialized microtex\n");
    if (!priv->error) {
      g_critical("uninitialized microtex and error state\n");
      goto cairo_ctx_free;
    }
    priv->error->ui_handler(ctx);
  } else {
    microtex::Graphics2D_cairo g2(ctx);
    priv->microtex->draw(g2, 0, 0);
  }
cairo_ctx_free:
  cairo_destroy(ctx);
}

static void microtex_equation_create_render(MicroTexEquation* self) {
  MicroTexEquationPrivate* priv = MICROTEX_EQUATION_GET_PRIVATE(self);
  GtkWidget* widget = GTK_WIDGET(self);

  if (!priv->shown) return;

  if (priv->microtex) delete priv->microtex;

  if (priv->error) {
    delete priv->error;
    priv->error = NULL;
  }

  if (priv->equation) try {
      priv->microtex = tex::MicroTeX::parse(
        std::string(priv->equation),
        microtex_equation_get_width(self),
        microtex_equation_get_text_size(self),
        microtex_equation_get_text_size(self) / 3.f,
        microtex_equation_get_color(self)
      );

      if (gtk_widget_get_has_tooltip(widget)) gtk_widget_set_has_tooltip(widget, FALSE);

    } catch (tex::ex_parse& e) {
      g_warning("MicroTeX parsing error: %s\n", e.what());

      gtk_widget_set_tooltip_text(widget, e.what());

      priv->error = new MicroTeXErrorState(
        e,
        +[](cairo_t* ctx) {
          cairo_set_source_rgb(ctx, 1, 0, 0);
          cairo_set_font_size(ctx, 10);
          cairo_move_to(ctx, 0, 14);
          cairo_show_text(ctx, "LaTeX parse failed");

          cairo_fill(ctx);
        }
      );

      priv->microtex = NULL;
    } catch (tex::ex_invalid_state& e) {
      g_warning("MicroTeX rendering error: %s\n", e.what());

      gtk_widget_set_tooltip_text(widget, e.what());

      priv->error = new MicroTeXErrorState(
        e,
        +[](cairo_t* ctx) {
          cairo_set_source_rgb(ctx, 1, 0, 0);
          cairo_set_font_size(ctx, 10);
          cairo_move_to(ctx, 0, 14);
          cairo_show_text(ctx, "LaTeX render failed");

          cairo_fill(ctx);
        }
      );

      priv->microtex = NULL;
    } catch (tex::ex_tex& e) {
      g_warning("Unknown MicroTeX error: %s\n", e.what());

      gtk_widget_set_tooltip_text(widget, e.what());

      priv->error = new MicroTeXErrorState(
        e,
        +[](cairo_t* ctx) {
          cairo_set_source_rgb(ctx, 1, 0, 0);
          cairo_set_font_size(ctx, 10);
          cairo_move_to(ctx, 0, 14);
          cairo_show_text(ctx, "Unknown MicroTeX error");

          cairo_fill(ctx);
        }
      );

      priv->microtex = NULL;
    }
  else
    priv->microtex = NULL;

  gtk_widget_queue_resize(GTK_WIDGET(self));
}

void microtex_equation_show(GtkWidget* widget) {
  MicroTexEquationPrivate* priv = MICROTEX_EQUATION_GET_PRIVATE(widget);
  priv->shown = TRUE;
  microtex_equation_create_render(MICROTEX_EQUATION(widget));
  GTK_WIDGET_CLASS(microtex_equation_parent_class)->show(widget);
}
void microtex_equation_hide(GtkWidget* widget) {
  MicroTexEquationPrivate* priv = MICROTEX_EQUATION_GET_PRIVATE(widget);
  priv->shown = FALSE;
  GTK_WIDGET_CLASS(microtex_equation_parent_class)->hide(widget);
}

static void microtex_equation_class_init(MicroTexEquationClass* klass) {
  GObjectClass* object_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->get_property = microtex_equation_get_property;
  object_class->set_property = microtex_equation_set_property;
  object_class->dispose = microtex_equation_dispose;

  widget_class->get_request_mode = microtex_equation_get_request_mode;
  widget_class->measure = microtex_equation_measure;
  widget_class->snapshot = microtex_equation_snapshot;

  widget_class->show = microtex_equation_show;
  widget_class->hide = microtex_equation_hide;

  // clang-format off
  /**
   * MicroTexEquation:latex: (attributes org.gtk.Property.get=microtex_equation_get_equation org.gtk.Property.set=microtex_equation_set_equation)
   *
   * The (La)TeX string that the widget will render and display.
   */
  // clang-format on
  obj_properties[PROP_EQUATION] = g_param_spec_string(
    "latex",
    "LaTeX",
    "LaTeX equation",
    "",
    (GParamFlags)(G_PARAM_READWRITE | G_PARAM_CONSTRUCT)
  );
  // clang-format off
  /**
   * MicroTexEquation:width: (attributes org.gtk.Property.get=microtex_equation_get_width org.gtk.Property.set=microtex_equation_set_width)
   *
   * the width of the 2D graphics context (in pixel) to limit the
   * formula layout, the engine will trying to wrap the layout if it
   * overflows the width, but will fails if formula cannot be split.
   */
  // clang-format on
  obj_properties[PROP_WIDTH] = g_param_spec_float(
    "width",
    "Width",
    "The maximal width the LaTeX equation may use horizontally",
    -1,
    G_MAXFLOAT,
    -1,
    (GParamFlags)(G_PARAM_READWRITE | G_PARAM_CONSTRUCT)
  );
  // clang-format off
  /**
   * MicroTexEquation:text-size: (attributes org.gtk.Property.get=microtex_equation_get_text_size org.gtk.Property.set=microtex_equation_set_text_size)
   *
   * the text size that the widget will be rendered with.
   */
  // clang-format on
  obj_properties[PROP_TEXT_SIZE] = g_param_spec_float(
    "text-size",
    "Textsize",
    "The text size to draw",
    6,
    G_MAXFLOAT,
    18,
    (GParamFlags)(G_PARAM_READWRITE | G_PARAM_CONSTRUCT)
  );
  // clang-format off
  /**
   * MicroTexEquation:color: (attributes org.gtk.Property.get=microtex_equation_get_color org.gtk.Property.set=microtex_equation_set_color)
   *
   * the foreground color, in which the equation (unless specifically
   * set otherweise) will be rendered with.
   *
   * The color is saved in an unsigned 32bit (4 bytes) integer in the following format:
   * ```c
   * struct Color {
   *     guint8 alpha;
   *     guint8 red;
   *     guint8 green;
   *     guint8 blue;
   * };
   * ```
   * with `0xFF` representing the full channel and `0x00` none.
   * Converting from GdkRGBA could look like this:
   * ```c
   * guint32 color_from_rgba(GdkRGBA color) {
   *     return
   * (guint8)(color.alpha*255)<<24|(guint8)(color.red*255)<<16|(guint8)(color.green*255)<<8|(guint8)(color.blue*255);
   * }
   * ```
   */
  // clang-format on
  obj_properties[PROP_COLOR] = g_param_spec_uint(
    "color",
    "Color",
    "The foreground color to draw",
    0,
    G_MAXUINT32,
    0xff000000,
    (GParamFlags)(G_PARAM_READWRITE | G_PARAM_CONSTRUCT)
  );
  g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

static void microtex_equation_init(MicroTexEquation* self) {
  MicroTexEquationPrivate* priv = MICROTEX_EQUATION_GET_PRIVATE(self);
  priv->shown = TRUE;
  priv->equation = NULL;
  priv->microtex = NULL;
  priv->width = -1;
  priv->text_size = 18;
  priv->color = 0xff000000;

  microtex_equation_init_tex();
}

extern "C" {
/**
 * microtex_equation_new:
 *
 * Creates a new `MicroTexEquation` widget.
 *
 * Returns (transfer full): a new `MicroTexEquation`
 */
GtkWidget* microtex_equation_new(void) {
  return static_cast<GtkWidget*>(g_object_new(MICROTEX_TYPE_EQUATION, NULL));
}

/**
 * microtex_equation_get_equation: (attributes org.gtk.Method.get_property=latex)
 * @self: a `MicroTexEquation`
 *
 * Returns the equation (La)TeX string being displayed by this widget.
 *
 * The reference count on the buffer is not incremented; the caller
 * of this function won’t own a new reference.
 *
 * Returns: (nullable) (transfer none): an UTF-8 string. This string is
 * owned by the widget and must not be modified or freed.
 */
const gchar* microtex_equation_get_equation(MicroTexEquation* self) {
  MicroTexEquationPrivate* priv;
  g_return_val_if_fail(MICROTEX_IS_EQUATION(self), NULL);
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  return priv->equation;
}
/**
 * microtex_equation_set_equation: (attributes org.gtk.Method.set_property=latex)
 * @self: a `MicroTexEquation`
 * @equation: a valid (La)TeX equation as UTF-8 string
 *
 * Sets the rendered equation of the widget to @equation.
 *
 * This will override any previously set equations.
 */
void microtex_equation_set_equation(MicroTexEquation* self, const gchar* equation) {
  MicroTexEquationPrivate* priv;
  g_return_if_fail(MICROTEX_IS_EQUATION(self));
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  if (priv->equation == equation) return;

  if (g_strcmp0(priv->equation, equation) == 0) return;

  if (priv->equation) g_free(priv->equation);

  priv->equation = g_strdup(equation);

  microtex_equation_create_render(self);

  g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_EQUATION]);
}

/**
 * microtex_equation_get_width: (attributes org.gtk.Method.get_property=width)
 * @self: a `MicroTexEquation`
 *
 * Returns the width that the graphic context uses to determine when to
 * break the line within the equation.
 *
 * See [property@MicroTex.Equation:width].
 *
 * Returns: an IEEE 764 floating point number. `0` when infinitly long
 */
gfloat microtex_equation_get_width(MicroTexEquation* self) {
  MicroTexEquationPrivate* priv;
  g_return_val_if_fail(MICROTEX_IS_EQUATION(self), NAN);
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  return priv->width;
}

/**
 * microtex_equation_set_width: (attributes org.gtk.Method.set_property=width)
 * @self: a `MicroTexEquation`
 * @width: width in IEEE 754 floating point format
 *
 * Sets the width that the context uses to determine when to linebreak.
 *
 * See [property@MicroTex.Equation:width].
 */
void microtex_equation_set_width(MicroTexEquation* self, gfloat width) {
  MicroTexEquationPrivate* priv;
  g_return_if_fail(MICROTEX_IS_EQUATION(self));
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  if (priv->width == width) return;

  priv->width = width;

  microtex_equation_create_render(self);

  g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_WIDTH]);
}

/**
 * microtex_equation_get_text_size: (attributes org.gtk.Method.get_property=text-size)
 * @self: a `MicroTexEquation`
 *
 * Returns the current text size.
 *
 * See [property@MicroTex.Equation:text-size].
 *
 * Returns: an IEEE 764 floating point number representing the text
 * size
 */
gfloat microtex_equation_get_text_size(MicroTexEquation* self) {
  MicroTexEquationPrivate* priv;
  g_return_val_if_fail(MICROTEX_IS_EQUATION(self), NAN);
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  return priv->text_size;
}

/**
 * microtex_equation_set_text_size: (attributes org.gtk.Method.set_property=text-size)
 * @self: a `MicroTexEquation`
 * @text_size: text size in IEEE 754 floating point format
 *
 * Sets the text size
 *
 * See [property@MicroTex.Equation:text-size].
 */
void microtex_equation_set_text_size(MicroTexEquation* self, gfloat text_size) {
  MicroTexEquationPrivate* priv;
  g_return_if_fail(MICROTEX_IS_EQUATION(self));
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  if (priv->text_size == text_size) return;

  priv->text_size = text_size;

  if (priv->microtex) {
    priv->microtex->setTextSize(priv->text_size);
    gtk_widget_queue_resize(GTK_WIDGET(self));
  }

  g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_TEXT_SIZE]);
}

/**
 * microtex_equation_get_color: (attributes org.gtk.Method.get_property=color)
 * @self: a `MicroTexEquation`
 *
 * Returns the current color of the equation.
 *
 * See [property@MicroTex.Equation:color] for the format.
 *
 * Returns: an unsigned 32bit integer. The format is defined in
 * [property@MicroTex.Equation:color].
 */
guint32 microtex_equation_get_color(MicroTexEquation* self) {
  MicroTexEquationPrivate* priv;
  g_return_val_if_fail(MICROTEX_IS_EQUATION(self), 0xff000000);
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  return priv->color;
}

/**
 * microtex_equation_set_color: (attributes org.gtk.Method.set_property=color)
 * @self: a `MicroTexEquation`
 * @color: color in the format specified in [property@MicroTex.Equation:color]
 *
 * Sets the color
 *
 * See [property@MicroTex.Equation:color].
 */
void microtex_equation_set_color(MicroTexEquation* self, guint32 color) {
  MicroTexEquationPrivate* priv;
  g_return_if_fail(MICROTEX_IS_EQUATION(self));
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  if (priv->color == color) return;

  priv->color = color;

  if (priv->microtex) {
    priv->microtex->setForeground(priv->color);
    gtk_widget_queue_draw(GTK_WIDGET(self));
  }

  g_object_notify_by_pspec(G_OBJECT(self), obj_properties[PROP_COLOR]);
}

/**
 * microtex_equation_get_baseline:
 * @self: a `MicroTexEquation`
 *
 * Gets the baseline of the rendered Equation in GTK format
 *
 * Returns: the baseline as signed 64bit integer
 */
gint64 microtex_equation_get_baseline(MicroTexEquation* self) {
  MicroTexEquationPrivate* priv;
  g_return_val_if_fail(MICROTEX_IS_EQUATION(self), -1);
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  if (!priv->microtex) return -1;

  int height = priv->microtex->getHeight();
  gint64 baseline = (gint64)round(0 + height * (1.0f - priv->microtex->getBaseline()));

  // workaround for not entirely correct baseline arithmetic
  if (height > 12) baseline += 1;

  return baseline;
}

static cairo_status_t array_writer_fun(GArray** array, const guint8* data, uint len) {
  *array = g_array_append_vals(*array, data, len);
  return CAIRO_STATUS_SUCCESS;
}
/**
 * microtex_equation_to_svg:
 * @self: a `MicroTexEquation`
 * @success (nullable): stores %TRUE if the rendered equation was valid
 *
 * Converts the equation to svg format.
 *
 * Keep in mind, that this method can store %FALSE in `@success` and
 * still return a `GBytes`. In these cases the svg will contain the
 * same error message that the widget itself also renders.
 * It is up to the discretion of the implementor whether to continue
 * working with the image or to discard (and free) it.
 *
 * Returns (nullable) (transfer full): the SVG as `GBytes` for future consumption
 */
GBytes* microtex_equation_to_svg(MicroTexEquation* self, gboolean* success) {
  MicroTexEquationPrivate* priv;
  g_return_val_if_fail(MICROTEX_IS_EQUATION(self), NULL);
  priv = MICROTEX_EQUATION_GET_PRIVATE(self);

  GByteArray* array = g_byte_array_new();
  cairo_surface_t* svg = cairo_svg_surface_create_for_stream(
    (cairo_write_func_t)array_writer_fun,
    &array,
    (double)priv->microtex->getWidth(),
    (double)priv->microtex->getHeight()
  );
  cairo_t* ctx = cairo_create(svg);

  if (!priv->microtex) {
    g_info("uninitialized microtex\n");
    if (success) *success = FALSE;
    if (!priv->error) {
      g_critical("uninitialized microtex and error state\n");
      cairo_destroy(ctx);
      cairo_surface_destroy(svg);
      g_byte_array_unref(array);
      return NULL;
    }
    priv->error->ui_handler(ctx);
  } else {
    microtex::Graphics2D_cairo g2(ctx);
    priv->microtex->draw(g2, 0, 0);
    if (success) *success = TRUE;
  }

  cairo_destroy(ctx);
  cairo_surface_destroy(svg);

  return g_byte_array_free_to_bytes(array);
}

/**
 * microtex_equation_init_tex:
 *
 * Initializes the µTeX renderer
 *
 * `MicroTexEquation` will call this function during init, so you
 * typically don't need to call this function yourself.
 */
gboolean microtex_equation_init_tex() {
  if (tex::MicroTeX::isInited()) {
    g_info("MicroTeX already initialized\n");
    return FALSE;
  }

  tex::InitFontSenseAuto autoinit;
  tex::MicroTeX::init(autoinit);
  tex::PlatformFactory::registerFactory("gtk", std::make_unique<tex::PlatformFactory_cairo>());
  tex::PlatformFactory::activate("gtk");
  tex::MicroTeX::overrideTexStyle(true, tex::TexStyle::display);

  return TRUE;
}
}
