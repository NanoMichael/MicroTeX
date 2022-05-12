import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:microtex/microtex.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await MicroTeX.instance.initialize(
    clmAsset: 'assets/XITSMath-Regular.clm2',
    textLayoutSerif: 'Noto Serif',
    textLayoutSansSerif: 'Noto Sans',
    textLayoutMonospace: 'Noto Mono',
  );
  debugPaintBaselinesEnabled = true;
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
        body: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Container(
                color: Colors.red[100],
                child: const LaTeX(
                  latex: "\\LaTeX\\MicroTeX\\AndroidTeX",
                  textSize: 50,
                  color: Colors.teal,
                ),
              ),
              const Text(
                "Hello world!🥰",
                style: TextStyle(
                  fontSize: 50,
                  fontFamily: "Comic Mono",
                ),
              ),
              RichText(
                text: TextSpan(
                  children: [
                    const TextSpan(
                      text: "Hello!",
                      style: TextStyle(color: Colors.black, fontSize: 50),
                    ),
                    WidgetSpan(
                      alignment: PlaceholderAlignment.baseline,
                      baseline: TextBaseline.alphabetic,
                      child: Container(
                        color: Colors.red[100],
                        child: const LaTeX(
                          latex: "x=\\frac{-b\\pm\\sqrt{b^2-4ac}}{2a}\\debug{}",
                          textSize: 50,
                          style: TeXStyle.display,
                          color: Colors.teal,
                        ),
                      ),
                    )
                  ],
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
