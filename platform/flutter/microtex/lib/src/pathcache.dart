import 'dart:ui';

class PathCache {
  static final instance = PathCache._();

  PathCache._();

  final _map = <int, Path>{};

  Path? operator [](int id) => _map[id];

  void operator []=(int id, Path path) => _map[id] = path;
}
