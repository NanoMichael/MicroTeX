#ifndef __ISAMPLES_H__
#define __ISAMPLES_H__

#include <glib.h>

G_BEGIN_DECLS

gpointer samples_new(const gchar* path);
void samples_free(gpointer samples);
gboolean samples_empty(gpointer samples);
const gchar* samples_next(gpointer samples);

G_END_DECLS

#endif  // __ISAMPLES_H__
