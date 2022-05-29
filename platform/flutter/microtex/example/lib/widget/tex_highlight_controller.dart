import 'package:flutter/material.dart';

class TeXHighlightController extends TextEditingController {
  TeXHighlightController({String? text}) : super(text: text);

  static final out = RegExp(r'\${1,2}[\s\S]*?\${1,2}');
  static final cmd = RegExp(r'\${1,2}|\\[a-zA-Z]*|\_|\^|\{|\}');

  @override
  TextSpan buildTextSpan({
    required BuildContext context,
    TextStyle? style,
    required bool withComposing,
  }) {
    final sty = style ?? const TextStyle();
    final dollarSty = sty.copyWith(color: Colors.deepOrange);
    final texStyle = sty.copyWith(color: Colors.green);
    final cmdStyle = sty.copyWith(color: Colors.blue);
    final keyStyle = sty.copyWith(color: Colors.purple);

    final children = <TextSpan>[];

    matchTeX(String tex) {
      tex.splitMapJoin(
        cmd,
        onNonMatch: (span) {
          if (span.isEmpty) return span;
          children.add(TextSpan(text: span, style: texStyle));
          return span;
        },
        onMatch: (m) {
          final cmd = m[0] ?? '';
          if (cmd.isEmpty) return '';
          final TextStyle s;
          if (cmd == '\$' || cmd == '\$\$') {
            s = dollarSty;
          } else if (cmd == '_' || cmd == '^') {
            s = keyStyle;
          } else if (cmd == '{' || cmd == '}') {
            s = sty;
          } else {
            s = cmdStyle;
          }
          children.add(TextSpan(text: cmd, style: s));
          return cmd;
        },
      );
    }

    text.splitMapJoin(
      out,
      onNonMatch: (span) {
        if (span.isEmpty) return span;
        children.add(TextSpan(text: span, style: style));
        return span;
      },
      onMatch: (m) {
        final tex = m[0] ?? '';
        if (tex.isEmpty) return '';
        matchTeX(tex);
        return tex;
      },
    );
    return TextSpan(style: style, children: children);
  }
}
