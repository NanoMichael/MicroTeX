#include <glib.h>

#include "samples.h"

extern "C" {
gpointer samples_new(const gchar* path) {
  if (!(path && *path))
    return NULL;
  std::string cxxpath(path);
  return (gpointer) new microtex::Samples(cxxpath);
}
void samples_free(gpointer samples) {
  delete (microtex::Samples*)samples;
}
gboolean samples_empty(gpointer samples) {
  return ((microtex::Samples*)samples)->isEmpty();
}
const gchar* samples_next(gpointer samples) {
  return ((microtex::Samples*)samples)->next().c_str();
}
}
