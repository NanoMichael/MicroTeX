import 'dart:ffi';
import 'dart:io';
import 'package:ffi/ffi.dart';
import 'package:microtex/src/context.dart';
import 'package:microtex/src/fontdesc.dart';

import 'bounds.dart';

typedef NCreateTextLayout = Uint32 Function(Pointer<Utf8>, Pointer<FontDesc>);
typedef NGetTextLayoutBounds = Void Function(Uint32, Pointer<Bounds>);
typedef NReleaseTextLayout = Void Function(Uint32);
typedef NIsPathExists = Bool Function(Uint32);
typedef NRegisterCallbacks = Void Function(
  Pointer<NativeFunction<NCreateTextLayout>>,
  Pointer<NativeFunction<NGetTextLayoutBounds>>,
  Pointer<NativeFunction<NReleaseTextLayout>>,
  Pointer<NativeFunction<NIsPathExists>>,
);
typedef RegisterCallbacks = void Function(
  Pointer<NativeFunction<NCreateTextLayout>>,
  Pointer<NativeFunction<NGetTextLayoutBounds>>,
  Pointer<NativeFunction<NReleaseTextLayout>>,
  Pointer<NativeFunction<NIsPathExists>>,
);

typedef NativeParse = Pointer Function(Pointer<Utf8>, Int32, Float, Float, Uint32, Bool, Bool, Uint32);
typedef Parse = Pointer Function(Pointer<Utf8>, int, double, double, int, bool, bool, int);

class NativeBindings {
  late final DynamicLibrary _nativeLib;

  // context
  late final RegisterCallbacks registerCallbacks;
  late final Pointer Function(int, Pointer) init;
  late final bool Function() isInited;
  late final void Function() release;
  late final bool Function() isLittleEndian;

  // font operations
  late final Pointer<Utf8> Function(Pointer) getFontFamily;
  late final Pointer<Utf8> Function(Pointer) getFontName;
  late final bool Function(Pointer) isMathFont;
  late final void Function(Pointer) releaseFontMeta;
  late final Pointer Function(int len, Pointer) addFont;

  // setters
  late final void Function(Pointer<Utf8>) setDefaultMathFont;
  late final void Function(Pointer<Utf8>) setDefaultMainFont;

  // parse
  late final Parse parse;

  // render
  late final void Function(Pointer) deleteRender;
  late final Pointer<Uint8> Function(Pointer, int, int) getDrawingData;
  late final void Function(Pointer) deleteDrawingData;
  late final int Function(Pointer) getRenderWidth;
  late final int Function(Pointer) getRenderHeight;
  late final int Function(Pointer) getRenderDepth;
  late final bool Function(Pointer) isRenderSplit;
  late final void Function(Pointer, double) setRenderTextSize;
  late final void Function(Pointer, int) setRenderForground;

  // glyph settings
  late final bool Function() hasGlyphPathRender;
  late final void Function(bool) setRenderGlyphUsePath;
  late final bool Function() isRenderGlyphUsePath;

  NativeBindings._() {
    _init();
  }

  factory NativeBindings() => NativeBindings._();

  void _init() {
    if (Platform.isLinux || Platform.isAndroid) {
      _nativeLib = DynamicLibrary.open('libmicrotex.so');
    } else if (Platform.isIOS || Platform.isMacOS) {
      _nativeLib = DynamicLibrary.process();
    } else if (Platform.isWindows) {
      _nativeLib = DynamicLibrary.open('microtex.dll');
    } else {
      // should never happen
    }
    if (debugMicroTeX) print(_nativeLib);
    // context
    registerCallbacks = _nativeLib
        .lookup<NativeFunction<NRegisterCallbacks>>(
          'microtex_registerCallbacks',
        )
        .asFunction<RegisterCallbacks>();
    init = _nativeLib
        .lookup<NativeFunction<Pointer Function(Uint32, Pointer)>>(
          "microtex_init",
        )
        .asFunction<Pointer Function(int, Pointer)>();
    isInited = _nativeLib
        .lookup<NativeFunction<Bool Function()>>(
          "microtex_isInited",
        )
        .asFunction();
    release = _nativeLib
        .lookup<NativeFunction<Void Function()>>(
          "microtex_release",
        )
        .asFunction();
    isLittleEndian = _nativeLib
        .lookup<NativeFunction<Bool Function()>>(
          "microtex_isLittleEndian",
        )
        .asFunction();
    // FontMeta
    getFontFamily = _nativeLib
        .lookup<NativeFunction<Pointer<Utf8> Function(Pointer)>>(
          "microtex_getFontFamily",
        )
        .asFunction();
    getFontName = _nativeLib
        .lookup<NativeFunction<Pointer<Utf8> Function(Pointer)>>(
          "microtex_getFontName",
        )
        .asFunction();
    isMathFont = _nativeLib
        .lookup<NativeFunction<Bool Function(Pointer)>>(
          "microtex_isMathFont",
        )
        .asFunction();
    releaseFontMeta = _nativeLib
        .lookup<NativeFunction<Void Function(Pointer)>>(
          "microtex_releaseFontMeta",
        )
        .asFunction();
    addFont = _nativeLib
        .lookup<NativeFunction<Pointer Function(Uint32, Pointer)>>(
          "microtex_addFont",
        )
        .asFunction();
    // setters
    setDefaultMathFont = _nativeLib
        .lookup<NativeFunction<Void Function(Pointer<Utf8>)>>(
          "microtex_setDefaultMathFont",
        )
        .asFunction();
    setDefaultMainFont = _nativeLib
        .lookup<NativeFunction<Void Function(Pointer<Utf8>)>>(
          "microtex_setDefaultMainFont",
        )
        .asFunction();
    // parse
    parse = _nativeLib
        .lookup<NativeFunction<NativeParse>>(
          "microtex_parseRender",
        )
        .asFunction();
    // render
    deleteRender = _nativeLib
        .lookup<NativeFunction<Void Function(Pointer)>>(
          "microtex_deleteRender",
        )
        .asFunction();
    getDrawingData = _nativeLib
        .lookup<NativeFunction<Pointer<Uint8> Function(Pointer, Int32, Int32)>>(
          "microtex_getDrawingData",
        )
        .asFunction();
    deleteDrawingData = _nativeLib
        .lookup<NativeFunction<Void Function(Pointer)>>(
          "microtex_freeDrawingData",
        )
        .asFunction();
    getRenderWidth = _nativeLib
        .lookup<NativeFunction<Int32 Function(Pointer)>>(
          "microtex_getRenderWidth",
        )
        .asFunction();
    getRenderHeight = _nativeLib
        .lookup<NativeFunction<Int32 Function(Pointer)>>(
          "microtex_getRenderHeight",
        )
        .asFunction();
    getRenderDepth = _nativeLib
        .lookup<NativeFunction<Int32 Function(Pointer)>>(
          "microtex_getRenderDepth",
        )
        .asFunction();
    isRenderSplit = _nativeLib
        .lookup<NativeFunction<Bool Function(Pointer)>>(
          "microtex_isRenderSplit",
        )
        .asFunction();
    setRenderTextSize = _nativeLib
        .lookup<NativeFunction<Void Function(Pointer, Float)>>(
          "microtex_setRenderTextSize",
        )
        .asFunction();
    setRenderForground = _nativeLib
        .lookup<NativeFunction<Void Function(Pointer, Uint32)>>(
          "microtex_setRenderForeground",
        )
        .asFunction();
    // glyph settings
    hasGlyphPathRender = _nativeLib
        .lookup<NativeFunction<Bool Function()>>(
          "microtex_hasGlyphPathRender",
        )
        .asFunction();
    setRenderGlyphUsePath = _nativeLib
        .lookup<NativeFunction<Void Function(Bool)>>(
          "microtex_setRenderGlyphUsePath",
        )
        .asFunction();
    isRenderGlyphUsePath = _nativeLib
        .lookup<NativeFunction<Bool Function()>>(
          "microtex_isRenderGlyphUsePath",
        )
        .asFunction();
  }
}
