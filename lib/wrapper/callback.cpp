#ifdef HAVE_CWRAPPER

#include "wrapper/callback.h"

F_createTextLayout microtex_createTextLayout = nullptr;
F_getTextLayoutBounds microtex_getTextLayoutBounds = nullptr;
F_releaseTextLayout microtex_releaseTextLayout = nullptr;
F_isPathExists microtex_isPathExists = nullptr;

#endif
