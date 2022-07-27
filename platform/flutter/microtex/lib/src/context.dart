import 'dart:ffi';

import 'package:flutter/cupertino.dart';
import 'package:flutter/foundation.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/services.dart';
import 'package:microtex/microtex.dart';
import 'package:microtex/src/bindings.dart';
import 'package:microtex/src/bounds.dart';
import 'package:microtex/src/fontdesc.dart';
import 'package:microtex/src/pathcache.dart';
import 'package:microtex/src/textlayout.dart';

bool _debugMicroTeX = false;

set debugMicroTeX(bool value) => _debugMicroTeX = value;

bool get debugMicroTeX => _debugMicroTeX && kDebugMode;

/// The MicroTeX context wrapper for Flutter from native. You must call [initialize]
/// to initialize the context before using.
class MicroTeX {
  MicroTeX._();

  static final _instance = MicroTeX._();

  factory MicroTeX() => _instance;

  late final bool _isLittleEndian;
  final _bindings = NativeBindings();
  final _fonts = <FontMeta>[];

  String _currentMathFontName = "";

  /// The name of the math font currently in use, empty if the context was
  /// not intialized.
  String get currentMathFontName => _currentMathFontName;

  set currentMathFontName(value) {
    if (_currentMathFontName == value) return;
    _currentMathFontName = value;
    _setMathFont(value);
  }

  String _currentMainFontFamilyName = "";

  /// The family name of the main font currently in use, if it is empty, that means
  /// no main font was given, thus the context fallbacks to the math font.
  String get currentMainFontFamilyName => _currentMainFontFamilyName;

  set currentMainFontFamilyName(value) {
    if (_currentMainFontFamilyName == value) return;
    _currentMainFontFamilyName = value;
    _setMainFont(value);
  }

  List<FontMeta> get mathFonts {
    return _fonts.where((element) => element.isMathFont).toList();
  }

  List<FontMeta> get mainFonts {
    return _fonts.where((element) => !element.isMathFont).toList();
  }

  /// Initialize the MicroTeX context.
  Future<void> initialize({
    required String clmAsset,
    String textLayoutSerif = "",
    String textLayoutSansSerif = "",
    String textLayoutMonospace = "",
  }) async {
    TextLayout.setFontFamily(
      textLayoutSerif,
      textLayoutSansSerif,
      textLayoutMonospace,
    );
    _isLittleEndian = _bindings.isLittleEndian();
    _registerCallbacks();
    await _initWithCLM(clmAsset);
  }

  void _registerCallbacks() {
    final register = _bindings.registerCallbacks;
    final a = Pointer.fromFunction<Uint32 Function(Pointer, Pointer)>(
      _createTextLayout,
      0,
    );
    final b = Pointer.fromFunction<Void Function(Uint32, Pointer)>(
      _getTextLayoutBounds,
    );
    final c = Pointer.fromFunction<Void Function(Uint32)>(
      TextLayout.release,
    );
    final d = Pointer.fromFunction<Bool Function(Uint32)>(
      _isPathExists,
      false,
    );
    register(a, b, c, d);
    if (debugMicroTeX) print(register);
  }

  static int _createTextLayout(Pointer<Utf8> txt, Pointer<FontDesc> font) {
    return TextLayout.create(txt.toDartString(), font.ref);
  }

  static void _getTextLayoutBounds(int id, Pointer<Bounds> bounds) {
    final b = TextLayout.getBounds(id);
    bounds.ref
      ..width = b[0]
      ..height = b[1]
      ..ascent = b[2];
  }

  static bool _isPathExists(int id) => PathCache()[id] != null;

  Future<FontMeta> _loadCLM(
    String clmAsset,
    Pointer Function(int len, Pointer<Uint8> buf) f,
  ) async {
    final clm = await rootBundle.load(clmAsset);
    final len = clm.lengthInBytes;
    // copy data to C
    final cbuf = calloc.allocate<Uint8>(len);
    final list = cbuf.asTypedList(len);
    list.setAll(0, clm.buffer.asUint8List());
    // callback
    final m = f(len, cbuf);
    // release buffer in C
    calloc.free(cbuf);
    // get font meta info from C
    final fontFamily = _bindings.getFontFamily(m).toDartString();
    final fontName = _bindings.getFontName(m).toDartString();
    final isMathFont = _bindings.isMathFont(m);
    // push to font meta list
    final meta = FontMeta(fontFamily, fontName, isMathFont);
    if (debugMicroTeX) print(meta);
    if (meta.isValid) _fonts.add(meta);
    // release meta pointer in C
    _bindings.releaseFontMeta(m);
    return meta;
  }

  Future<void> _initWithCLM(String clmAsset) async {
    final meta = await _loadCLM(clmAsset, (len, buf) => _bindings.init(len, buf));
    currentMathFontName = meta.fontName;
  }

  /// Add a clm font from assets.
  Future<FontMeta> addFont(String clmAsset) async {
    return _loadCLM(clmAsset, (len, buf) => _bindings.addFont(len, buf));
  }

  void _setMathFont(String name) {
    final str = name.toNativeUtf8();
    _bindings.setDefaultMathFont(str);
    calloc.free(str);
  }

  void _setMainFont(String familyName) {
    final str = familyName.toNativeUtf8();
    _bindings.setDefaultMainFont(str);
    calloc.free(str);
  }

  /// Test if MicroTeX has the ability to use path to render glyphs.
  bool hasGlyphPathRender() {
    return _bindings.hasGlyphPathRender();
  }

  /// Set if use path to render glyphs, only works if [hasGlyphPathRender] returns true,
  /// otherwise this function takes no effects.
  void setRenderGlyphUsePath(bool use) {
    _bindings.setRenderGlyphUsePath(use);
  }

  /// Test if currently use path to render glyphs.
  bool isRenderGlyphUsePath() {
    return _bindings.isRenderGlyphUsePath();
  }

  Render parse({
    required String tex,
    required int width,
    required double textSize,
    required double lineSpace,
    required Color color,
    required bool fillWidth,
    required bool overrideTeXStyle,
    required TeXStyle style,
  }) {
    final str = tex.toNativeUtf8();
    final ptr = _bindings.parse(
      str,
      width,
      textSize,
      lineSpace,
      color.value,
      fillWidth,
      overrideTeXStyle,
      style.index,
    );
    calloc.free(str);
    return Render(_bindings, ptr, _isLittleEndian);
  }
}
