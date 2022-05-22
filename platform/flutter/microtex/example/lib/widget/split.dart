import 'package:flutter/material.dart';

class SplitView extends StatefulWidget {
  final Widget left;
  final Widget right;
  final double ratio;
  final double dividerThickness;

  const SplitView({
    Key? key,
    required this.left,
    required this.right,
    this.ratio = 0.5,
    this.dividerThickness = 16,
  }) : super(key: key);

  @override
  State<SplitView> createState() => _SplitViewState();
}

class _SplitViewState extends State<SplitView> {
  double _ratio = 0;

  @override
  void initState() {
    super.initState();
    _ratio = widget.ratio;
  }

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(builder: (context, constraints) {
      final maxWidth = constraints.maxWidth - widget.dividerThickness;
      final lw = maxWidth * _ratio;
      final rw = maxWidth - lw;
      return SizedBox(
        width: constraints.maxWidth,
        child: Row(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            SizedBox(width: lw, child: widget.left),
            GestureDetector(
              behavior: HitTestBehavior.translucent,
              onPanUpdate: (details) {
                setState(() {
                  _ratio += details.delta.dx / maxWidth;
                  if (_ratio > 1) {
                    _ratio = 1;
                  } else if (_ratio < 0) {
                    _ratio = 0;
                  }
                });
              },
              child: SizedBox(
                width: widget.dividerThickness,
                height: constraints.maxHeight,
                child: Container(
                  color: Colors.black26,
                  child: const RotationTransition(
                    child: Icon(Icons.drag_handle),
                    turns: AlwaysStoppedAnimation(0.25),
                  ),
                ),
              ),
            ),
            SizedBox(width: rw, child: widget.right),
          ],
        ),
      );
    });
  }
}
