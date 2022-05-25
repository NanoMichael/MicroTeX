extension StringExt on String {
  String takeUntil(bool Function(int j, String char) stop, [int from = 0, bool inclusive = false]) {
    for (var i = from; i < length; i++) {
      if (stop(i, this[i])) {
        return substring(from, inclusive ? i + 1 : i);
      }
    }
    return '';
  }
}