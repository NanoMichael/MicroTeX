import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_spinbox/material.dart';
import 'package:microtex/microtex.dart';
import 'package:microtex_example/widget/rich.dart';
import 'package:microtex_example/widget/split.dart';
import 'package:microtex_example/widget/tex_highlight_controller.dart';

class _Settings {
  String mode;
  String mainFontFamily;
  String mathFont;
  double textSize;

  _Settings(this.mode, this.mainFontFamily, this.mathFont, this.textSize);
}

class EditScreen extends StatefulWidget {
  const EditScreen({Key? key}) : super(key: key);

  @override
  State<EditScreen> createState() => _EditScreenState();
}

class _EditScreenState extends State<EditScreen> {
  final _controller = StreamController<String>();

  @override
  dispose() {
    super.dispose();
    _controller.close();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('MicroTeX Example'),
        actions: [
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: IconButton(
              icon: const Icon(Icons.settings),
              onPressed: () => _showSettings(context),
            ),
          ),
        ],
      ),
      body: SplitView(
        left: _buildEditor(),
        right: _buildRich(),
      ),
    );
  }

  _section(
    String title,
    String selected,
    List<String> sections,
    void Function(String? value) onChanged,
  ) {
    return [
      Text(
        title,
        style: const TextStyle(fontWeight: FontWeight.w500, fontSize: 18),
      ),
      ...sections.map(
        (e) => ListTile(
          title: Text(e),
          horizontalTitleGap: 0,
          contentPadding: const EdgeInsets.only(left: 8),
          leading: Radio(value: e, groupValue: selected, onChanged: onChanged),
        ),
      )
    ];
  }

  _textSize() {
    return SpinBox(
      min: 10,
      max: 200,
      value: 20,
      decoration: const InputDecoration(labelText: 'TextSize'),
      onChanged: (value) => {},
    );
  }

  _settings() {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      mainAxisSize: MainAxisSize.min,
      children: [
        ..._section('Mode', 'text', ['text', 'LaTeX'], (value) {}),
        ..._section(
          'Math Font',
          MicroTeX().currentMathFontName,
          MicroTeX().mathFonts.map((e) => e.fontName).toList(),
          (value) {},
        ),
        ..._section(
          'Main Font',
          MicroTeX().currentMainFontFamilyName.isEmpty ? 'none' : MicroTeX().currentMainFontFamilyName,
          MicroTeX().mainFonts.map((e) => e.fontFamily).toSet().toList() + ['none'],
          (value) {},
        ),
        Container(
          constraints: const BoxConstraints(maxWidth: 300),
          padding: const EdgeInsets.only(left: 10),
          child: const Text(
            'If you just want to use the math font to render the text '
            'wrapped by command "text", just set it to "none".',
          ),
        ),
        const SizedBox(height: 20),
        _textSize(),
      ],
    );
  }

  Future<void> _showSettings(BuildContext context) async {
    final navigator = Navigator.of(context);
    final settings = await showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text('Settings'),
          content: StatefulBuilder(builder: (context, setState) => _settings()),
          actions: [
            TextButton(
              child: const Text('OK'),
              onPressed: () {
                navigator.pop('test');
              },
            ),
          ],
        );
      },
    );
  }

  _buildRich() {
    return SingleChildScrollView(
      primary: false,
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: StreamBuilder(
          stream: _controller.stream,
          builder: (context, AsyncSnapshot<String> snapshot) {
            final data = snapshot.data;
            if (data == null) {
              return const Text(
                'Input sth...',
                style: TextStyle(fontSize: 25),
              );
            }
            return SimpleRichText(text: data);
          },
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
            child: FutureBuilder(
              future: rootBundle.loadString('assets/EXAMPLE.txt').then((value) {
                _controller.sink.add(value);
                return value;
              }),
              builder: (context, AsyncSnapshot<String> snapshot) {
                return _buildTextField(snapshot.data);
              },
            ),
          ),
        );
      }),
    );
  }

  _buildTextField(String? txt) {
    final controller = TeXHighlightController(text: txt);
    // final controller = TeXInputController(text: txt);
    return TextField(
      controller: controller,
      keyboardType: TextInputType.multiline,
      maxLines: null,
      style: const TextStyle(
        fontFamily: 'Monaco',
        fontFamilyFallback: ['Noto Color Emoji'],
      ),
      decoration: const InputDecoration(
        border: InputBorder.none,
        contentPadding: EdgeInsets.all(16),
      ),
      onChanged: (text) => _controller.sink.add(text),
    );
  }
}
