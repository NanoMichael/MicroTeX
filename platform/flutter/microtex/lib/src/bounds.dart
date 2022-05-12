import 'dart:ffi';

class Bounds extends Struct {
  @Float()
  external double width;
  @Float()
  external double height;
  @Float()
  external double ascent;
}
