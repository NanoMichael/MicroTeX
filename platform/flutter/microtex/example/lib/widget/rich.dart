import 'package:flutter/material.dart';
import 'package:microtex/microtex.dart';

class SimpleRichText extends StatelessWidget {
  final String text;

  const SimpleRichText({
    Key? key,
    required this.text,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return RichText(
      text: TextSpan(children: _buildSpans()),
    );
  }

  static const r1 = r'\$\$[\s\S]*?\$\$';
  static const r2 = r'\$[\s\S]*?\$';

  _buildSpans() {
    final r = RegExp([r1, r2].join('|'));
    final children = <InlineSpan>[];
    text.splitMapJoin(
      r,
      onNonMatch: (span) {
        if (span.isEmpty) return span;
        children.add(
          TextSpan(
            text: span,
            style: const TextStyle(color: Colors.black, fontSize: 20),
          ),
        );
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
            child: _buildLaTeX(tex, isInline),
          ),
        ));
        return tex.toString();
      },
    );
    return children;
  }

  _buildLaTeX(String tex, bool isInline) {
    final t = LaTeX(
      latex: tex,
      textSize: 24,
      color: Colors.black,
      blur: const Blur(BlurStyle.normal, 0.34),
    );
    if (isInline) {
      return t;
    }
    return LayoutBuilder(builder: (context, constraints) {
      return SizedBox(
        width: constraints.maxWidth,
        child: Center(child: t),
      );
    });
  }
}
