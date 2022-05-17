import 'package:flutter/material.dart';
import 'package:flutter/painting.dart';
import 'package:microtex/src/context.dart';
import 'package:microtex/src/fontdesc.dart';

class TextLayout {
  TextLayout._();

  static String _serif = "";
  static String _sansSerif = "";
  static String _monospace = "";

  static int _id = 0;
  static final _texts = <int, Txt>{};

  static void setFontFamily(String serif, String sansSerif, String monospace) {
    _serif = serif;
    _sansSerif = sansSerif;
    _monospace = monospace;
  }

  static int create(String text, FontDesc font) {
    _id++;
    _texts[_id] = Txt(text, font);
    return _id;
  }

  /// FIXME
  /// Because TextStyle cannot be modified, we need to create 2 painters in
  /// method [getBounds] to retrieve the metrics and method [draw] to
  /// change the color to draw
  static TextPainter _createPainter(Txt t, [Color? color]) {
    return TextPainter(
      text: TextSpan(
        text: t.txt,
        style: TextStyle(
          fontSize: t.fontSize,
          fontWeight: t.fontWeight,
          fontStyle: t.fontStyle,
          fontFamily: t.fontFamily,
          color: color,
        ),
      ),
      textDirection: TextDirection.ltr,
    );
  }

  static List<double> getBounds(int id) {
    final t = _texts[id];
    if (t == null) return [0, 0, 0];
    final painter = _createPainter(t)..layout();
    final m = painter.computeLineMetrics().first;
    t.ascent = m.ascent;
    return [m.width, m.ascent + m.descent, -m.ascent];
  }

  static void draw(int id, Canvas canvas, double x, double y, Color color) {
    final t = _texts[id];
    if (t == null) return;
    _createPainter(t, color)
      ..layout()
      ..paint(canvas, Offset(x, y - t.ascent));
  }

  static void release(int id) {
    if (debugMicroTeX) {
      print('TextLayout.relase($id)');
    }
    _texts.remove(id);
  }
}

class Txt {
  final String txt;
  final double fontSize;
  late final FontWeight fontWeight;
  late final FontStyle fontStyle;
  late final String fontFamily;

  double ascent = 0;

  Txt(this.txt, FontDesc font) : fontSize = font.fontSize {
    if (font.isSansSerif) {
      fontFamily = TextLayout._sansSerif;
    } else if (font.isMonospace) {
      fontFamily = TextLayout._monospace;
    } else {
      fontFamily = TextLayout._serif;
    }
    fontWeight = font.isBold ? FontWeight.bold : FontWeight.normal;
    fontStyle = font.isItalic ? FontStyle.italic : FontStyle.normal;
  }
}
