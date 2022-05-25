import 'package:flutter/material.dart';

class TeXHighlightController extends TextEditingController {
  TeXHighlightController({String? text}) : super(text: text);

  @override
  TextSpan buildTextSpan({
    required BuildContext context,
    TextStyle? style,
    required bool withComposing,
  }) {
    final dollarStyle = style?.copyWith(color: Colors.amberAccent) ??
        const TextStyle(color: Colors.amberAccent);
    final texStyle = style?.copyWith(color: Colors.green) ??
        const TextStyle(color: Colors.green);
    final cmdStyle = style?.copyWith(color: Colors.blue) ??
        const TextStyle(color: Colors.blue);

    final children = <TextSpan>[];
    var k = 0;
    var isInTeX = false;
    for (var i = 0; i < text.length; i++) {
      final c = text[i];
      if (c == '\$') {
        if (!isInTeX && k != i) {
          final str = text.substring(k, i);
          children.add(TextSpan(text: str, style: style));
        }
        var j = i + 1;
        // consume the '$'
        while (j < text.length && text[j] == '\$') {
          j++;
        }
        i = j;
        final str = text.substring(i, j);
        if (str.length <= 2) {
          isInTeX = !isInTeX;
        }
        children.add(TextSpan(text: str, style: dollarStyle));
        k = i;
      }
    }
    return TextSpan(style: style, children: children);
  }
}
