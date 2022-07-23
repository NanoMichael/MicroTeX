import 'dart:ui';

class PathCache {
  static final _instance = PathCache._();

  PathCache._();

  factory PathCache() {
    return _instance;
  }

  final _map = <int, Path>{};

  Path? operator [](int id) => _map[id];

  void operator []=(int id, Path path) => _map.putIfAbsent(id, () => path);
}
