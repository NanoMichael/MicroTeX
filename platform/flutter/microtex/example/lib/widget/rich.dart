import 'package:flutter/material.dart';
import 'package:microtex_example/utils/convert_spans.dart';

class SimpleRichText extends StatelessWidget {
  final String text;

  const SimpleRichText({
    Key? key,
    required this.text,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return RichText(
      text: TextSpan(
        children: SpanConverter().buildSpans(
          text: text,
          style: const TextStyle(color: Colors.black, fontSize: 20),
        ),
      ),
    );
  }
}
