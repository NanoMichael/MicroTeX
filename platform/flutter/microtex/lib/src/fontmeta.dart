class FontMeta {
  final String fontFamily;
  final String fontName;
  final bool isMathFont;

  FontMeta(this.fontFamily, this.fontName, this.isMathFont);

  @override
  String toString() {
    return "FontMeta{$fontFamily, $fontName, $isMathFont}";
  }
}
