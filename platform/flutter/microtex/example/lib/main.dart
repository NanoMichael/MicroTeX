import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';

import 'package:microtex/microtex.dart';
import 'package:microtex_example/screen/edit_screen.dart';
import 'package:microtex_example/widget/cmp.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await MicroTeX().initialize(
    clmAsset: 'assets/xits/XITSMath-Regular.clm2',
    textLayoutSerif: 'Noto Color Emoji',
    textLayoutSansSerif: 'Noto Color Emoji',
    textLayoutMonospace: 'Noto Color Emoji',
  );
  debugMicroTeX = true;
  runApp(
    const MaterialApp(
      debugShowCheckedModeBanner: false,
      home: EditScreen(),
    ),
  );
}
