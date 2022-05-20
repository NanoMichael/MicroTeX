import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:microtex/microtex.dart';
import 'package:microtex_example/widget/vertical_split.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await MicroTeX.instance.initialize(
    clmAsset: 'assets/XITSMath-Regular.clm2',
    textLayoutSerif: 'Noto Serif',
    textLayoutSansSerif: 'Noto Sans',
    textLayoutMonospace: 'Noto Mono',
  );
  debugMicroTeX = true;
  debugPaintSizeEnabled = false;
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text("MicroTeX example app"),
        ),
        body: VerticalSplitView(
          left: _buildEditor(),
          right: _buildCmp(),
        ),
      ),
    );
  }

  _buildEditor() {
    return ConstrainedBox(
      constraints: const BoxConstraints.expand(),
      child: LayoutBuilder(builder: (context, constraints) {
        return SingleChildScrollView(
          child: ConstrainedBox(
            constraints: BoxConstraints(
              minWidth: constraints.maxWidth,
              minHeight: constraints.maxHeight,
            ),
            child: const TextField(
              keyboardType: TextInputType.multiline,
              maxLines: null,
              style: TextStyle(fontFamily: 'monospace'),
              decoration: InputDecoration(
                border: InputBorder.none,
                contentPadding: EdgeInsets.all(16),
              ),
            ),
          ),
        );
      }),
    );
  }

  _buildCmp() {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        _buildRow(
          'render by text',
          () => const Text(
            'Hello World!',
            style: TextStyle(fontSize: 100, fontFamily: 'XITS Math'),
          ),
        ),
        _buildRow(
          'render by path with blur filter',
          () => const LaTeX(
            latex: "\\text{Hello World!}",
            textSize: 100,
            style: TeXStyle.text,
            color: Colors.black,
            filterSigma: 0.5,
          ),
        ),
        const SizedBox(
          height: 25,
        ),
        _buildRow(
          'render by path without blur filter',
          () => const LaTeX(
            latex: "\\text{Hello World!}",
            textSize: 100,
            style: TeXStyle.text,
            color: Colors.black,
          ),
        ),
        const SizedBox(height: 50),
        _buildRichText(),
      ],
    );
  }

  _buildRow(String desc, Widget Function() build) {
    return Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        SizedBox(
          width: 150,
          child: Text(
            desc,
            textAlign: TextAlign.right,
            style: const TextStyle(fontSize: 18),
          ),
        ),
        const SizedBox(width: 10),
        build(),
      ],
    );
  }

  _buildRichText() {
    return RichText(
      text: const TextSpan(
        children: [
          TextSpan(
            text: "Hello!",
            style: TextStyle(
              color: Colors.black,
              fontSize: 25,
              fontFamily: 'XITS Math',
            ),
          ),
          WidgetSpan(
            alignment: PlaceholderAlignment.baseline,
            baseline: TextBaseline.alphabetic,
            child: LaTeX(
              latex: "F(\\omega)=\\int_{-\\infty}^{+\\infty}f(t)e^{-i\\omega_nt}dt",
              textSize: 25,
              style: TeXStyle.text,
              color: Colors.black,
              filterSigma: 0.34,
            ),
          )
        ],
      ),
    );
  }
}
