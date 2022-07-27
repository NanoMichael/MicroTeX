import 'package:flutter/material.dart';
import 'package:microtex/microtex.dart';

class SpanConverter {
  SpanConverter._();

  static final _r = RegExp(r'\${1,2}[\s\S]*?\${1,2}');
  static final _instance = SpanConverter._();

  factory SpanConverter() {
    return _instance;
  }

  List<InlineSpan> buildSpans({
    required String text,
    required TextStyle style,
  }) {
    final children = <InlineSpan>[];
    text.splitMapJoin(
      _r,
      onNonMatch: (span) {
        if (span.isEmpty) return span;
        children.add(TextSpan(text: span, style: style));
        return span;
      },
      onMatch: (m) {
        final tex = m[0] ?? '';
        if (tex.isEmpty) return '';
        final isInline = !tex.startsWith('\$\$');
        children.add(WidgetSpan(
          alignment: PlaceholderAlignment.baseline,
          baseline: TextBaseline.alphabetic,
          child: Padding(
            padding: const EdgeInsets.symmetric(vertical: 8),
            child: _buildLaTeX(tex, isInline, style),
          ),
        ));
        return tex.toString();
      },
    );
    return children;
  }

  _buildLaTeX(String tex, bool isInline, TextStyle style) {
    final t = LaTeX(
      latex: tex,
      textSize: style.fontSize ?? 14,
      color: style.color ?? Colors.black,
      style: TeXStyle.display,
      isRenderGlyphUsePath: false,
    );
    if (isInline) {
      return t;
    }
    return LayoutBuilder(builder: (context, constraints) {
      print('build: ${constraints.maxWidth}');
      return SizedBox(
        width: constraints.maxWidth,
        child: Center(child: t),
      );
    });
  }
}
