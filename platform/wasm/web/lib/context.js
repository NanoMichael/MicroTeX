'use strict'

import {initRuntime} from "./runtime";
import {Render} from './render';
import {createTextLayout, getTextLayoutBounds, releaseTextLayout} from "./textlayout";

const context = {};

/**
 * The loaded main font
 *
 * @type {Set<string>}
 * @private
 */
let _mainFontFamilies = new Set();

/**
 * The loaded math font names.
 *
 * @type {Set<string>}
 * @private
 */
let _mathFontNames = new Set();

/**
 * The current math font name.
 *
 * @type {string}
 * @private
 */
let _currentMathFontName = "";

/**
 * The current main font family.
 *
 * @type {string}
 * @private
 */
let _currentMainFontFamily = "";

/**
 * The byte order
 *
 * @type {boolean}
 * @private
 */
let _isLittleEndian = false;

/**
 * The wasm runtime
 *
 * @type {Object}
 * @private
 */
let _runtime = null;

/**
 * Copy data from an array buffer to native heap and
 * return the pointer of the heap memory. You must
 * call {@link freeHeap} after this memory is unused.
 *
 * @param {ArrayBuffer} arrayBuffer
 * @returns {number} the pointer to the heap memory
 */
function copyToHeap(arrayBuffer) {
  const numBytes = arrayBuffer.byteLength;
  const ptr = _runtime._malloc(numBytes);
  const heapBytes = new Uint8Array(
    _runtime.HEAPU8.buffer,
    ptr, numBytes
  );
  heapBytes.set(new Uint8Array(arrayBuffer));
  return ptr;
}

/**
 * Release heap memory.
 *
 * @param {number} ptr the pointer to the heap memory
 */
function freeHeap(ptr) {
  if (ptr === undefined || !ptr) return;
  _runtime._free(ptr);
}

/**
 * Load clm data from uri
 *
 * @param {string} clmDataUri the clm data uri
 * @param {function(number, number)} f callback after clm data was loaded
 */
function loadClm(clmDataUri, f) {
  return fetch(clmDataUri)
    .then(res => res.arrayBuffer())
    .then(buf => {
      const ptr = copyToHeap(buf);
      try {
        const len = buf.byteLength;
        return [len, f(len, ptr)]
      } finally {
        freeHeap(ptr);
      }
      return [0, 0]
    })
    .then(([len, meta]) => {
      if (!meta) return;
      // get font meta info
      const cname = _runtime._microtex_getFontName(meta);
      const fontName = _runtime.UTF8ToString(cname);
      const isMathFont = _runtime._microtex_isMathFont(meta);

      if (isMathFont) {
        _mathFontNames.add(fontName);
        if (_currentMathFontName === "") {
          _currentMathFontName = fontName;
        }
        console.log(`add math font, '${fontName}', size: ${len}`)
      } else {
        const cstr = _runtime._microtex_getFontFamily(meta);
        const familyName = _runtime.UTF8ToString(cstr);
        _mainFontFamilies.add(familyName);
        if (_currentMainFontFamily === "") {
          _currentMainFontFamily = familyName;
        }
        console.log(`add main font, '{${familyName}, ${fontName}}', size: ${len}`)
      }
      // do not forget to release the meta info
      _runtime._microtex_releaseFontMeta(meta);
    });
}

/**
 * Init the context with given clm data uri.
 *
 * @param {string} clmDataUri the clm data uri
 * @returns {Promise<void>} a promise to init the context
 */
context.init = function (clmDataUri) {
  if (_runtime != null && _runtime._microtex_isInited()) {
    throw new Error("the context was initialized already.");
  }
  return initRuntime()
    .then(r => _runtime = r)
    .then(_ => {
      _runtime._microtex_setRenderGlyphUsePath(true);
    })
    .then(_ => {
      const a = _runtime.addFunction(createTextLayout, 'iii');
      const b = _runtime.addFunction(getTextLayoutBounds, 'vii');
      const c = _runtime.addFunction(releaseTextLayout, 'vi');
      const d = _runtime.addFunction((id) => false, 'ii');
      _runtime._microtex_registerCallbacks(a, b, c, d);
    })
    .then(_ => loadClm(
      clmDataUri,
      (len, data) => _runtime._microtex_init(len, data))
    )
    .then(_ => {
      _isLittleEndian = _runtime._microtex_isLittleEndian();
    });
}

/** Release the context. */
context.release = function () {
  _runtime._microtex_release();
}

/** Check if context is initialized. */
context.isInited = function () {
  return _runtime._microtex_isInited();
}

/**
 * Add a font to context.
 *
 * @param {string} clmDataUri the clm data uri
 */
context.addFont = function (clmDataUri) {
  return loadClm(
    clmDataUri,
    (len, ptr) => _runtime._microtex_addFont(len, ptr)
  );
}

/**
 * Set main font to render formulas. The font MUST hava been added to the context.
 *
 * @param {string} familyName the family name, the engine will use math font
 * to render text if is empty.
 * @throws {TypeError} if the font was not added.
 */
context.setMainFont = function (familyName) {
  if (familyName !== "" && !_mainFontFamilies.has(familyName)) {
    throw new TypeError("the font family `" + familyName + "` has no font.");
  }
  const cstr = _runtime.allocateUTF8(familyName);
  try {
    _runtime._microtex_setDefaultMainFont(cstr);
    _currentMainFontFamily = familyName;
  } finally {
    freeHeap(cstr);
  }
  console.log(`set main font: ${familyName}`);
}

/**
 * Set math font to render formulas. The font MUST have been added to the context.
 *
 * @param {string} fontName the font name
 * @throws {TypeError} if the font was not added
 */
context.setMathFont = function (fontName) {
  if (!_mathFontNames.has(fontName)) {
    throw new TypeError("the font `" + fontName + "` was not added.");
  }
  const cstr = _runtime.allocateUTF8(fontName);
  try {
    _runtime._microtex_setDefaultMathFont(cstr);
    _currentMathFontName = fontName;
  } finally {
    freeHeap(cstr);
  }
  console.log(`set math font: ${fontName}`);
}

/**
 * Get the math fonts added to the context, return an empty array if the
 * context was not initialized.
 *
 * @return {string[]} the copy of the loaded math fonts
 */
context.getMathFontNames = function () {
  return [..._mathFontNames];
}

/**
 * Get the main font family names added to the context, returns an empty
 * array if no main font was added.
 *
 * @return {string[]} the copy of the loaded main font family names
 */
context.getMainFontFamilyNames = function () {
  return [..._mainFontFamilies];
}

/**
 * Parse a LaTeX formatted string.
 *
 * @param {string} tex the LaTeX formatted string to be parsed
 * @param {number} width the width of the graphical context (in pixel)
 * @param {number} textSize the text size to paint the formula (in pixel)
 * @param {number} lineSpace the line space to layout multi-lines formulas (in pixel)
 * @param {number} color \#AARRGGBB formatted color
 * @param {boolean} fillWidth whether fill the graphical context if is in inter-line mode
 * @returns {Render} a render to paint the parsed formula
 */
context.parse = function (
  tex, width, textSize,
  lineSpace, color, fillWidth = true
) {
  const cstr = _runtime.allocateUTF8(tex);
  let ptr = 0;
  try {
    ptr = _runtime._microtex_parseRender(cstr, width, textSize, lineSpace, color, fillWidth);
  } finally {
    freeHeap(cstr);
  }
  return new Render(ptr, _isLittleEndian);
}

export {context};
