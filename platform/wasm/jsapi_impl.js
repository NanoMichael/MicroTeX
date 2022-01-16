mergeInto(LibraryManager.library, {
  /**
   * Create a text layout from given text and font.
   *
   * @param {Number} src the string pointer from native side
   * @param {Number} font the font style string pointer
   * @return {number} the layout id
   */
  js_createTextLayout: function (src, font) {
    // Initialize the global textLayout if not defined
    if (typeof Module.textLayout === "undefined") {
      let textLayout = {};
      const canvas = document.createElement("canvas");
      textLayout.ctx = canvas.getContext('2d');
      textLayout.map = new Map();
      textLayout.id = 0;
      Module.textLayout = textLayout;
    }

    const txt = UTF8ToString(src);
    const css = UTF8ToString(font);

    const textLayout = Module.textLayout;
    const id = textLayout.id;
    textLayout.map.set(id, {
      font: css,
      text: txt
    });
    textLayout.id++;

    console.log(`js createTextLayout(${txt}, ${css})`);
    return id;
  },

  /**
   * Get text layout bounds.
   *
   * @param {Number} id the layout id
   * @param {Number} ptr the bounds buffer from native
   */
  js_getTextLayoutBounds: function (id, ptr) {
    const textLayout = Module.textLayout;
    // the value must be exists
    const value = textLayout.map.get(id);
    const ctx = textLayout.ctx;
    ctx.font = value.font;
    // measure the text and get its metrics
    const m = ctx.measureText(value.text);
    const v = new Float32Array(Module.HEAP8.buffer, ptr);
    v[0] = m.width;
    v[1] = m.actualBoundingBoxAscent + m.actualBoundingBoxDescent;
    v[2] = -m.actualBoundingBoxAscent;
    console.log(`js getTextLayoutBounds(${id}, ${ptr})`);
  },

  /**
   * Release current text layout.
   *
   * @param {Number} id the layout id
   */
  js_releaseTextLayout: function (id) {
    Module.textLayout.map.delete(id);
    console.log(`js releaseTextLayout(${id})`);
  }
});
