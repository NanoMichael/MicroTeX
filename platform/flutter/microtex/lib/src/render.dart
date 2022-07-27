import 'dart:convert';
import 'dart:ffi';
import 'dart:typed_data';
import 'dart:ui';

import 'package:flutter/rendering.dart';
import 'package:microtex/microtex.dart';
import 'package:microtex/src/bindings.dart';
import 'package:microtex/src/pathcache.dart';
import 'package:microtex/src/textlayout.dart';

class Render {
  final NativeBindings _bindings;
  final Pointer _nativeInstance;
  final Endian _endian;

  /// If filter is enabled, using a bluring to simulate the 'grid fitting' on glyhs
  /// drawing to get a much clear rendering, but it is expensive, especially for
  /// complex formulas.
  /// See [https://github.com/flutter/flutter/issues/104017].
  Blur? blur;

  Render(this._bindings, this._nativeInstance, bool isLittleEndian)
      : _endian = isLittleEndian ? Endian.little : Endian.big;

  /// The width of this render.
  int get width => _bindings.getRenderWidth(_nativeInstance);

  /// The height of this render, equals to ascent + descent (or namely [depth],
  /// distance below the baseline).
  int get height => _bindings.getRenderHeight(_nativeInstance);

  /// The distance below the baseline, in positive.
  int get depth => _bindings.getRenderDepth(_nativeInstance);

  /// Test if this render is split to multi-lines.
  bool get isSplit => _bindings.isRenderSplit(_nativeInstance);

  void release() {
    _bindings.deleteRender(_nativeInstance);
  }

  /// Set the text size to draw. The dimension ([width], [height], [depth]) will be
  /// changed following the text size change.
  void setTextSize(double textSize) {
    _bindings.setRenderTextSize(_nativeInstance, textSize);
  }

  /// Set the foreground color to draw.
  void setColor(int color) {
    _bindings.setRenderForground(_nativeInstance, color);
  }

  /// If this render is use path to render glyphs.
  ///
  /// Unlike the [MicroTeX.isRenderGlyphUsePath], this is for every particular renders.
  bool isRenderGlyphUsePath = false;

  static const _caps = [StrokeCap.butt, StrokeCap.round, StrokeCap.square];
  static const _joins = [StrokeJoin.bevel, StrokeJoin.round, StrokeJoin.miter];

  final _paint = Paint()..isAntiAlias = true;
  final _argBuf = Float32List(8);

  void draw(Canvas canvas) {
    print("Render.draw");
    // it is OK to change the value of the property [MicroTeX.isRenderGlyphUsePath] when
    // drawing every renders, since draw runs in UI thread.
    final prevIsRenderGlyphUsePath = MicroTeX().isRenderGlyphUsePath();
    MicroTeX().setRenderGlyphUsePath(isRenderGlyphUsePath);
    // get the drawing commands from native render
    final drawingData = _bindings.getDrawingData(_nativeInstance, 0, 0);
    // the first 4 bytes is the total count of byte used
    final n = drawingData.asTypedList(4);
    final len = n.buffer.asByteData().getUint32(0, _endian);

    // make a view to iterate over the drawing commands
    final v = drawingData.asTypedList(len).buffer.asByteData();
    var offset = 0;

    int getU8() {
      final x = v.getUint8(offset);
      offset += 1;
      return x;
    }

    T getNum<T>(int size, T Function(int offset) f) {
      final x = f(offset);
      offset += size;
      return x;
    }

    String getString() {
      final utf8 = <int>[];
      var byte = getU8();
      while (byte != 0) {
        utf8.add(byte);
        byte = getU8();
      }
      return const Utf8Decoder().convert(utf8);
    }

    int getU16() => getNum(2, (offset) => v.getUint16(offset, _endian));
    int getI32() => getNum(4, (offset) => v.getInt32(offset, _endian));
    int getU32() => getNum(4, (offset) => v.getUint32(offset, _endian));
    double getF32() => getNum(4, (offset) => v.getFloat32(offset, _endian));

    Float32List getF32s(int cnt) {
      for (var i = 0; i < cnt; i++) {
        _argBuf[i] = v.getFloat32(offset, _endian);
        offset += 4;
      }
      return _argBuf;
    }

    // consume the first 4 bytes
    getU32();
    var sx = 1.0, sy = 1.0;
    var path = Path();

    var fontFamily = "";
    var fontSize = 0.0;

    while (offset < len) {
      final cmd = getU8();
      switch (cmd) {
        case 0: // setColor
          final color = getU32();
          _paint.color = Color(color);
          break;
        case 1: // setStroke
          final lineWidth = getF32();
          final miterLimit = getF32();
          final cap = getU32();
          final join = getU32();
          _paint
            ..strokeWidth = lineWidth
            ..strokeMiterLimit = miterLimit
            ..strokeCap = _caps[cap]
            ..strokeJoin = _joins[join];
          break;
        case 2: // setDash
          final hasDash = getU8() == 1;
          final dash = hasDash ? [5 / sx, 5 / sx] : [];
          // FIXME
          // Flutter does not support dashing lines
          break;
        case 3: // setFont
          // TODO
          final family = getString();
          fontFamily = family;
          break;
        case 4: // setFontSize
          // TODO
          final size = getF32();
          fontSize = size;
          break;
        case 5: // translate
          final t = getF32s(2);
          canvas.translate(t[0], t[1]);
          break;
        case 6: // scale
          final s = getF32s(2);
          sx *= s[0];
          sy *= s[1];
          canvas.scale(s[0], s[1]);
          break;
        case 7: // rotate
          final r = getF32s(3);
          canvas.translate(r[1], r[2]);
          canvas.rotate(r[0]);
          canvas.translate(-r[1], -r[2]);
          break;
        case 8: // reset
          canvas.transform(Matrix4.identity().storage);
          break;
        case 9: // drawGlyph
          // TODO
          final glyph = getU16();
          final xy = getF32s(2);
          final run = GlyphRun(
            glyphs: Uint16List.fromList([glyph]),
            positions: [Offset(xy[0], xy[1])],
            fontFamily: fontFamily,
            fontSize: fontSize,
          );
          canvas.drawGlyphRun(run, const Offset(0, 0), _paint);
          break;
        case 10: // beginPath
          final pid = getI32();
          final p = PathCache()[pid];
          path = p ?? Path();
          break;
        case 11: // moveTo
          final m = getF32s(2);
          path.moveTo(m[0], m[1]);
          break;
        case 12: // lineTo
          final l = getF32s(2);
          path.lineTo(l[0], l[1]);
          break;
        case 13: // cubicTo
          final c = getF32s(6);
          path.cubicTo(c[0], c[1], c[2], c[3], c[4], c[5]);
          break;
        case 14: // quadTo
          final q = getF32s(4);
          path.quadraticBezierTo(q[0], q[1], q[2], q[3]);
          break;
        case 15: // closePath
          path.close();
          break;
        case 16: // fillPath
          final pid = getI32();
          _paint.style = PaintingStyle.fill;
          final b = blur;
          final p = b == null ? _paint : (_paint.copy()..maskFilter = MaskFilter.blur(b.style, b.sigma / sx));
          PathCache()[pid] = path;
          canvas.drawPath(path, p);
          break;
        case 17: // drawLine
          final dl = getF32s(4);
          canvas.drawLine(Offset(dl[0], dl[1]), Offset(dl[2], dl[3]), _paint);
          break;
        case 18: // drawRect
          final dr = getF32s(4);
          _paint.style = PaintingStyle.stroke;
          canvas.drawRect(Rect.fromLTWH(dr[0], dr[1], dr[2], dr[3]), _paint);
          break;
        case 19: // fillRect
          final fr = getF32s(4);
          _paint.style = PaintingStyle.fill;
          canvas.drawRect(Rect.fromLTWH(fr[0], fr[1], fr[2], fr[3]), _paint);
          break;
        case 20: // drawRoundRect
          final rr = getF32s(6);
          _paint.style = PaintingStyle.stroke;
          canvas.drawRRect(
            RRect.fromRectXY(Rect.fromLTWH(rr[0], rr[1], rr[2], rr[3]), rr[4], rr[5]),
            _paint,
          );
          break;
        case 21: // fillRoundRect
          final rf = getF32s(6);
          _paint.style = PaintingStyle.fill;
          canvas.drawRRect(
            RRect.fromRectXY(Rect.fromLTWH(rf[0], rf[1], rf[2], rf[3]), rf[4], rf[5]),
            _paint,
          );
          break;
        case 22: // drawTextLayout
          final id = getU32();
          final xy = getF32s(2);
          TextLayout.draw(id, canvas, xy[0], xy[1], _paint.color);
          break;
        default:
          // invalid drawing command
          break;
      }
    }
    // delete the drawing data
    _bindings.deleteDrawingData(drawingData);
    // reset the property
    MicroTeX().setRenderGlyphUsePath(prevIsRenderGlyphUsePath);
  }

  @override
  String toString() {
    return "Render{$width, $height, $depth}";
  }
}

extension _PaintExt on Paint {
  Paint copy() {
    return Paint()
      ..isAntiAlias = isAntiAlias
      ..color = color
      ..blendMode = blendMode
      ..style = style
      ..strokeWidth = strokeWidth
      ..strokeCap = strokeCap
      ..strokeJoin = strokeJoin
      ..strokeMiterLimit = strokeMiterLimit
      ..maskFilter = maskFilter
      ..filterQuality = filterQuality
      ..shader = shader
      ..colorFilter = colorFilter
      ..imageFilter = imageFilter
      ..invertColors = invertColors;
  }
}
