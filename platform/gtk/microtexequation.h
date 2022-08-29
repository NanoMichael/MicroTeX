#ifndef __MICROTEXEQUATION_H__
#define __MICROTEXEQUATION_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#ifdef _MSC_VER
#include "vcruntime.h"
#endif

G_BEGIN_DECLS

// msvc
#ifdef _MSC_VER
#if defined(MICROTEX_LIBRARY)
#define MICROTEX_EXPORT __declspec(dllexport)
#else
#define MICROTEX_EXPORT __declspec(dllimport)
#endif
// gnuc and clang
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#define MICROTEX_EXPORT __attribute((visibility("default")))
// otherwise...
#else
#define MICROTEX_EXPORT
#endif

#define MICROTEX_TYPE_EQUATION (microtex_equation_get_type())
MICROTEX_EXPORT
G_DECLARE_DERIVABLE_TYPE(MicroTexEquation, microtex_equation, MICROTEX, EQUATION, GtkWidget)

struct _MicroTexEquationClass {
  GtkWidgetClass parent_class;

  gpointer padding[12];
};

MICROTEX_EXPORT
GtkWidget* microtex_equation_new();

MICROTEX_EXPORT
const gchar* microtex_equation_get_equation(MicroTexEquation* self);
MICROTEX_EXPORT
void microtex_equation_set_equation(MicroTexEquation* self, const gchar* equation);

MICROTEX_EXPORT
gfloat microtex_equation_get_width(MicroTexEquation* self);
MICROTEX_EXPORT
void microtex_equation_set_width(MicroTexEquation* self, gfloat width);

MICROTEX_EXPORT
gfloat microtex_equation_get_text_size(MicroTexEquation* self);
MICROTEX_EXPORT
void microtex_equation_set_text_size(MicroTexEquation* self, gfloat text_size);

MICROTEX_EXPORT
guint32 microtex_equation_get_color(MicroTexEquation* self);
MICROTEX_EXPORT
void microtex_equation_set_color(MicroTexEquation* self, guint32 color);

MICROTEX_EXPORT
gint64 microtex_equation_get_baseline(MicroTexEquation* self);

MICROTEX_EXPORT
GBytes* microtex_equation_to_svg(MicroTexEquation* self, gboolean* success);

MICROTEX_EXPORT
gboolean microtex_equation_init_tex();

G_END_DECLS

#endif  // __MICROTEXEQUATION_H__
