import 'package:flutter/material.dart';
import 'package:microtex_example/utils/convert_spans.dart';

class TeXInputController extends TextEditingController {
  TeXInputController({String? text}) : super(text: text);

  @override
  TextSpan buildTextSpan({
    required BuildContext context,
    TextStyle? style,
    required bool withComposing,
  }) {
    final children = SpanConverter().buildSpans(
      text: text,
      style: style ?? const TextStyle(),
    );
    return TextSpan(style: style, children: children);
  }
}
