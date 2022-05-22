import 'dart:math';

import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';

class DragPane extends StatefulWidget {
  final Widget top;
  final Widget bottom;
  final double dividerThickness;
  final double offset;

  const DragPane({
    Key? key,
    required this.top,
    required this.bottom,
    this.dividerThickness = 16,
    this.offset = 20,
  }) : super(key: key);

  @override
  State<DragPane> createState() => _DragPaneState();
}

class _DragPaneState extends State<DragPane> {
  double _offset = 10;

  @override
  void initState() {
    super.initState();
    _offset = widget.offset;
  }

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(builder: (context, constraints) {
      return Stack(
        children: [
          Positioned.fill(child: widget.bottom),
          Positioned(
            top: _offset,
            left: 0,
            right: 0,
            bottom: 0,
            child: _buildTop(constraints),
          ),
        ],
      );
    });
  }

  _buildTop(BoxConstraints constraints) {
    final h = constraints.maxHeight - widget.dividerThickness - _offset;
    return Column(
      children: [
        GestureDetector(
          behavior: HitTestBehavior.translucent,
          onPanUpdate: (detailes) => setState(() {
            _offset += detailes.delta.dy;
            final b = constraints.maxHeight - widget.dividerThickness;
            _offset = max(0, min(_offset, b));
          }),
          child: SizedBox(
            width: constraints.maxWidth,
            height: widget.dividerThickness,
            child: Container(
              color: Colors.black26,
              child: const Icon(Icons.drag_handle),
            ),
          ),
        ),
        SizedBox(height: h, child: widget.top),
      ],
    );
  }
}
