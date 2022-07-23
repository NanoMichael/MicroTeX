class FontMeta {
  final String fontFamily;
  final String fontName;
  final bool isMathFont;

  FontMeta(this.fontFamily, this.fontName, this.isMathFont);

  bool get isValid => fontFamily.isNotEmpty && fontName.isNotEmpty;

  @override
  String toString() {
    return "FontMeta{$fontFamily, $fontName, $isMathFont}";
  }
}
