#ifdef HAVE_CWRAPPER

#include "wrapper/callback.h"

CBCreateTextLayout microtex_createTextLayout = nullptr;
CBGetTextLayoutBounds microtex_getTextLayoutBounds = nullptr;
CBReleaseTextLayout microtex_releaseTextLayout = nullptr;
CBIsPathExists microtex_isPathExists = nullptr;

#endif
