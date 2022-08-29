#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <microtexequation.h>

#include "isamples.h"
#include "mt_gtk_res.h"

static gchar* samples_file = NULL;

struct _MicroTexGtkDemoWin {
  GtkApplicationWindow parent_instance;

  GtkPaned* pane;
  GtkButton* sample_btn;
  GtkColorButton* color_btn;
  GtkSourceBuffer* buffer;
  MicroTexEquation* equation;
  GtkButton* save_btn;

  gpointer samples;
};

#define MICROTEX_TYPE_GTKDEMOWIN (microtex_gtkdemowin_get_type())
G_DECLARE_FINAL_TYPE(MicroTexGtkDemoWin, microtex_gtkdemowin, MICROTEX, GTKDEMOWIN, GtkApplicationWindow)
G_DEFINE_TYPE(MicroTexGtkDemoWin, microtex_gtkdemowin, GTK_TYPE_APPLICATION_WINDOW)

static void microtex_gtkdemowin_object_dispose(GObject* object) {
  MicroTexGtkDemoWin* self = MICROTEX_GTKDEMOWIN(object);

  if (self->samples)
    samples_free(g_steal_pointer(&self->samples));

  G_OBJECT_CLASS(microtex_gtkdemowin_parent_class)->dispose(object);
}
static void microtex_gtkdemowin_class_init(MicroTexGtkDemoWinClass* class) {
  GObjectClass* object_class = G_OBJECT_CLASS(class);
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

  object_class->dispose = microtex_gtkdemowin_object_dispose;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/NanoMichael/MicroTeX/GTKDemo/microtex-gtk.ui");
  gtk_widget_class_bind_template_child(widget_class, MicroTexGtkDemoWin, pane);
  gtk_widget_class_bind_template_child(widget_class, MicroTexGtkDemoWin, sample_btn);
  gtk_widget_class_bind_template_child(widget_class, MicroTexGtkDemoWin, color_btn);
  gtk_widget_class_bind_template_child(widget_class, MicroTexGtkDemoWin, buffer);
  gtk_widget_class_bind_template_child(widget_class, MicroTexGtkDemoWin, equation);
  gtk_widget_class_bind_template_child(widget_class, MicroTexGtkDemoWin, save_btn);
}

static void update_nextbtn(MicroTexGtkDemoWin* self) {
  if (self->samples) {
    if (!samples_empty(self->samples)) {
      gtk_widget_set_sensitive(GTK_WIDGET(self->sample_btn), TRUE);
      return;
    }
  }
  gtk_widget_set_sensitive(GTK_WIDGET(self->sample_btn), FALSE);
}
static void nextbtn_clicked(GtkButton* btn, MicroTexGtkDemoWin* self) {
  if (!self->samples)
    return;
  if (samples_empty(self->samples))
    return;

  const gchar* sample = samples_next(self->samples);
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(self->buffer), sample, -1);

  update_nextbtn(self);
}
static void colorbtn_changed(GtkColorButton* btn, MicroTexGtkDemoWin* self) {
  GdkRGBA gcolor;
  gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn), &gcolor);
  guint32 color = (guint8)(gcolor.alpha * 255) << 24 | (guint8)(gcolor.red * 255) << 16 | (guint8)(gcolor.green * 255) << 8 | (guint8)(gcolor.blue * 255);
  microtex_equation_set_color(self->equation, color);
}
static void save_svg_cb(GtkNativeDialog* diag, gint resp_id, GBytes* svg) {
  if (resp_id == GTK_RESPONSE_ACCEPT) {
    GFile* file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(diag));
    gssize svgdata_len;
    const guint8* svgdata = g_bytes_get_data(svg, &svgdata_len);
    GError* err = NULL;
    if (!g_file_replace_contents(file, svgdata, svgdata_len, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, &err)) {
      g_critical("Failure saving file: %s\n", err->message);
      g_error_free(err);
    }
    g_object_unref(file);
  }
  g_bytes_unref(svg);
}
static void save_svg_btn_clicked(GtkButton* btn, MicroTexGtkDemoWin* self) {
  gboolean success;
  GBytes* svg = microtex_equation_to_svg(self->equation, &success);

  if (!success) {
    g_warning("Current equation is not valid, aborting save.\n");
    g_bytes_unref(svg);
    return;
  }

  GtkFileChooserNative* fc = gtk_file_chooser_native_new("Open File", GTK_WINDOW(self), GTK_FILE_CHOOSER_ACTION_SAVE, "Save", "Cancel");
  g_signal_connect(fc, "response", G_CALLBACK(save_svg_cb), svg);
  GtkFileFilter* filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter, "Scalable Vector Graphics (SVG)");
  gtk_file_filter_add_mime_type(filter, "image/svg+xml");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc), filter);
  g_object_unref(filter);

  gtk_native_dialog_show(GTK_NATIVE_DIALOG(fc));
}
static void update_latex_pane_size(MicroTexGtkDemoWin* self) {
  GtkWidget* child = gtk_paned_get_end_child(self->pane);
  gint width = gtk_widget_get_allocated_width(child);
  microtex_equation_set_width(self->equation, width);
}
static void paned_position_changed(GtkPaned* pane, GParamSpec* pspec, MicroTexGtkDemoWin* self) {
  update_latex_pane_size(self);
}

static void microtex_gtkdemowin_init(MicroTexGtkDemoWin* self) {
  g_type_ensure(MICROTEX_TYPE_EQUATION);
  gtk_widget_init_template(GTK_WIDGET(self));

  self->samples = samples_new(samples_file);
  update_nextbtn(self);
  g_signal_connect(self->sample_btn, "clicked", G_CALLBACK(nextbtn_clicked), self);

  GtkStyleContext* style_ctx = gtk_widget_get_style_context(GTK_WIDGET(self->equation));
  GdkRGBA fgcolor;
  if (!gtk_style_context_lookup_color(style_ctx, "theme_text_color", &fgcolor)) {
    g_warning("current gtk theme does not provide `theme_text_color`.\n");
    fgcolor.red = 0.f;
    fgcolor.green = 0.f;
    fgcolor.blue = 0.f;
    fgcolor.alpha = 1.f;
  }
  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(self->color_btn), &fgcolor);
  g_signal_connect(self->color_btn, "color-set", G_CALLBACK(colorbtn_changed), self);
  colorbtn_changed(self->color_btn, self);

  g_signal_connect(self->save_btn, "clicked", G_CALLBACK(save_svg_btn_clicked), self);

  g_signal_connect(self->pane, "notify::position", G_CALLBACK(paned_position_changed), self);
  update_latex_pane_size(self);

  GtkSourceLanguageManager* langmgr = gtk_source_language_manager_get_default();
  GtkSourceLanguage* tex = gtk_source_language_manager_get_language(langmgr, "latex");
  gtk_source_buffer_set_language(self->buffer, tex);
}

static void activate(GtkApplication* app, gpointer user_data) {
  GtkWidget* win = g_object_new(MICROTEX_TYPE_GTKDEMOWIN, "application", app, NULL);
  gtk_widget_show(win);
}

static const GOptionEntry options[] = {
  {"samples", 's', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_FILENAME, &samples_file, "path to the SAMPLES.tex file", "\"res/SAMPLES.tex\""},
  {NULL}};
int main(int argc, char** argv) {
  GtkApplication* app;
  int status;

  g_resources_register(mt_gtk_resource_get_resource());
  app = gtk_application_new("com.github.nanomichael.microtex.gtkdemo", G_APPLICATION_FLAGS_NONE);
  g_application_add_main_option_entries(G_APPLICATION(app), options);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
