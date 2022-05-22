import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:microtex_example/widget/rich.dart';
import 'package:microtex_example/widget/split.dart';

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
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('MicroTeX Example'),
        ),
        body: SplitView(
          left: _buildEditor(),
          right: _buildRich(),
        ),
      ),
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
                  return TextField(
                    controller: TextEditingController()..text = snapshot.data ?? '',
                    keyboardType: TextInputType.multiline,
                    maxLines: null,
                    style: const TextStyle(
                      fontFamily: 'monospace',
                      fontFamilyFallback: ['Noto Color Emoji'],
                    ),
                    decoration: const InputDecoration(
                      border: InputBorder.none,
                      contentPadding: EdgeInsets.all(16),
                    ),
                    onChanged: (text) => _controller.sink.add(text),
                  );
                }),
          ),
        );
      }),
    );
  }
}
