//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <microtex/microtex_plugin.h>

void fl_register_plugins(FlPluginRegistry* registry) {
  g_autoptr(FlPluginRegistrar) microtex_registrar =
      fl_plugin_registry_get_registrar_for_plugin(registry, "MicrotexPlugin");
  microtex_plugin_register_with_registrar(microtex_registrar);
}
