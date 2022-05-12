import 'dart:ffi';

class FontDesc extends Struct {
  @Bool()
  external bool isBold;
  @Bool()
  external bool isItalic;
  @Bool()
  external bool isSansSerif;
  @Bool()
  external bool isMonospace;
  @Float()
  external double fontSize;
}
