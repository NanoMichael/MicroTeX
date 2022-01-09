#include "api.h"
#include "latex.h"
#include "render.h"

#ifdef __cplusplus
extern "C" {
#endif

void CLATEX_WASM_API add(
  unsigned long len,
  const unsigned char* data
) {
  printf("add array, len: %ld, %p\n", len, data);
  int sum = 0;
  for (size_t i = 0; i < len; i++) {
    printf("data[%ld] = %d\n", i, data[i]);
    sum += data[i];
  }
  printf("sum: %d\n", sum);
}

void CLATEX_WASM_API clatex_init(
  const char* name,
  unsigned long len,
  const unsigned char* data
) {
  printf("clatex init, data: %p\n", data);
  tex::LaTeX::init(name, len, data);
}

bool CLATEX_WASM_API clatex_isInited() {
  return tex::LaTeX::isInited();
}

long CLATEX_WASM_API clatex_parse(
  const char* tex,
  int width,
  float textSize,
  float lineSpace,
  unsigned int color
) {
  auto r = tex::LaTeX::parse(tex, width, textSize, lineSpace, color);
  return reinterpret_cast<long>(r);
}

void CLATEX_WASM_API clatex_delete(long render) {
  auto r = reinterpret_cast<tex::Render*>(render);
  delete r;
}

int main(int argc, char** argv) {
  printf("hello clatexmath\n");
  return 0;
}

#ifdef __cplusplus
};
#endif
