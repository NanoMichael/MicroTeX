import 'package:flutter/material.dart';
import 'package:microtex/microtex.dart';

class Blur {
  final BlurStyle style;
  final double sigma;

  const Blur(this.style, this.sigma);

  @override
  bool operator ==(Object other) {
    return other is Blur && style == other.style && sigma == other.sigma;
  }

  @override
  int get hashCode => hashValues(style, sigma);
}

class LaTeX extends LeafRenderObjectWidget {
  final String latex;
  final double textSize;
  final Color color;
  final TeXStyle? style;
  final Blur? blur;

  const LaTeX({
    Key? key,
    required this.latex,
    double? textSize,
    Color? color,
    this.style,
    this.blur,
  })  : textSize = textSize ?? 20,
        color = color ?? const Color(0xFF424242),
        super(key: key);

  @override
  RenderObject createRenderObject(BuildContext context) {
    return RenderLaTeX(latex, textSize, color, style, blur);
  }

  @override
  void updateRenderObject(BuildContext context, covariant RenderLaTeX renderObject) {
    renderObject.update(latex, textSize, color, style, blur);
  }
}

class RenderLaTeX extends RenderBox {
  String _latex;
  double _textSize;
  Color _color;
  TeXStyle? _style;
  Blur? _blur;

  double _lastMaxWidth = 0;
  Render? _render;

  RenderLaTeX(this._latex, this._textSize, this._color, this._style, this._blur);

  void update(String latex, double textSize, Color color, TeXStyle? style, Blur? blur) {
    var needParse = false;
    if (_latex != latex || _style != style) {
      _latex = latex;
      _style = style;
      needParse = true;
    }
    var needLayout = false;
    if (_textSize != textSize) {
      _textSize = textSize;
      _render?.setTextSize(textSize);
      needLayout = true;
    }
    var needPaint = false;
    if (_color != color || _blur != blur) {
      _color = color;
      _blur = blur;
      _render?.setColor(color.value);
      _render?.blur = blur;
      needPaint = true;
    }
    if (debugMicroTeX) {
      print(
        'RenderLaTeX.update'
        '{parse: $needParse, layout: $needLayout, paint: $needPaint}',
      );
    }
    if (needParse) {
      _render?.release();
      _render = null;
      markNeedsLayout();
      return;
    }
    if (needLayout) {
      markNeedsLayout();
      return;
    }
    if (needPaint) markNeedsPaint();
  }

  @override
  void dispose() {
    super.dispose();
    _render?.release();
  }

  @override
  void performLayout() {
    var r = _render;
    var reparse = false;
    final maxWidth = constraints.maxWidth;
    if (r != null && _lastMaxWidth != maxWidth) {
      // If max width changed, it might need to re-parse
      reparse = r.isSplit;
    }
    _lastMaxWidth = maxWidth;
    if (r == null || reparse) {
      if (debugMicroTeX) {
        print(
          'RenderLaTeX.performLayout'
          '{render: $r, reparse: $reparse}',
        );
      }
      r = MicroTeX().parse(
        _latex,
        _lastMaxWidth == double.infinity ? 0 : _lastMaxWidth.toInt(),
        _textSize,
        _textSize / 3,
        _color,
        false,
        // never fill the width, if you want to, wrap a Container or something like that
        _style != null,
        _style ?? TeXStyle.text,
      );
      r.blur = _blur;
      _render = r;
    }
    size = Size(r.width.toDouble(), r.height.toDouble());
  }

  @override
  double? computeDistanceToActualBaseline(TextBaseline baseline) {
    final r = _render;
    if (r == null) return null;
    return (r.height - r.depth).toDouble();
  }

  @override
  void paint(PaintingContext context, Offset offset) {
    final r = _render;
    if (r == null) return;
    context.canvas.translate(offset.dx, offset.dy);
    r.draw(context.canvas);
    context.canvas.translate(-offset.dx, -offset.dy);
  }
}
