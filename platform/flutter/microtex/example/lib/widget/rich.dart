import 'package:flutter/material.dart';
import 'package:microtex/microtex.dart';

class SimpleRichText extends StatelessWidget {
  final List<_Token> _tokens;

  SimpleRichText({
    Key? key,
    required String text,
  })  : _tokens = _parseTokens(text),
        super(key: key);

  @override
  Widget build(BuildContext context) {
    return RichText(
      text: TextSpan(
        children: _tokens.map(_convertToken).toList(),
      ),
    );
  }

  InlineSpan _convertToken(_Token e) {
    if (e.type == _TokenType.text) {
      return TextSpan(
        text: e.content,
        style: const TextStyle(
          color: Colors.black,
          fontSize: 20,
        ),
      );
    } else {
      return WidgetSpan(
        alignment: PlaceholderAlignment.baseline,
        baseline: TextBaseline.alphabetic,
        child: Padding(
          padding: const EdgeInsets.symmetric(vertical: 8),
          child: _buildLaTeX(e),
        ),
      );
    }
  }

  _buildLaTeX(_Token e) {
    final t = LaTeX(
      latex: e.content,
      textSize: 24,
      color: Colors.black,
      blur: const Blur(BlurStyle.normal, 0.34),
    );
    if (e.isInline) {
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

List<_Token> _parseTokens(String str) {
  final tokens = <_Token>[];
  var k = 0;
  for (var i = 0; i < str.length; i++) {
    final c = str[i];
    if (c == '\$') {
      var tex = '';
      var inline = true;
      if (i + 1 < str.length && str[i + 1] == '\$') {
        inline = false;
        tex = str.takeUntil((j, x) => j > i + 1 && x == '\$' && str[j - 1] == '\$', i, true);
      } else {
        inline = true;
        tex = str.takeUntil((j, x) => j > i && x == '\$', i, true);
      }
      if (tex.isEmpty) continue;
      if (k < i) {
        tokens.add(_Token(_TokenType.text, str.substring(k, i)));
        k = i;
      }
      tokens.add(_Token(_TokenType.tex, tex, inline));
      i += tex.length - 1;
      k += tex.length;
    }
  }
  if (k < str.length) {
    tokens.add(_Token(_TokenType.text, str.substring(k, str.length)));
  }
  return tokens;
}

enum _TokenType { text, tex }

class _Token {
  final _TokenType type;
  final bool isInline;
  final String content;

  _Token(this.type, this.content, [this.isInline = false]);
}

extension _StringExt on String {
  String takeUntil(bool Function(int j, String char) stop, [int from = 0, bool inclusive = false]) {
    for (var i = from; i < length; i++) {
      if (stop(i, this[i])) {
        return substring(from, inclusive ? i + 1 : i);
      }
    }
    return '';
  }
}
