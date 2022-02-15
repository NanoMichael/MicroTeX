'use strict'

import {initRuntime} from "./runtime";
import {Render} from './render';

let context = {};

/**
 * The loaded main font
 *
 * @type {String[]}
 * @private
 */
let _mainFonts = [];

/**
 * The loaded math font names.
 *
 * @type {String[]}
 * @private
 */
let _mathFonts = [];

/**
 * The current math font name.
 *
 * @type {string}
 * @private
 */
let _currentMathFont = "";

/**
 * The current main font name.
 *
 * @type {string}
 * @private
 */
let _currentMainFont = "";

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
 * @returns {Number} the pointer of the heap memory
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
 * @param {Number} ptr the pointer of the heap memory
 */
function freeHeap(ptr) {
  _runtime._free(ptr);
}

/**
 * Init the context with given clm data uri.
 *
 * @param {String} clmDataUri the clm data uri
 * @param {String} fontName the font name, default is "dft"
 * @returns {Promise<void>} a promise to init the context
 */
context.init = function (clmDataUri, fontName = "dft") {
  if (_runtime != null && _runtime._tinytex_isInited()) {
    throw new Error("the context was initialized already.");
  }
  return initRuntime()
    .then(r => _runtime = r)
    .then(_ => fetch(clmDataUri))
    .then(res => res.arrayBuffer())
    .then(buf => {
      console.log("init with '" + fontName + "' clm data size: " + buf.byteLength);
      const dft = _runtime.allocateUTF8(fontName);
      const ptr = copyToHeap(buf);
      try {
        _runtime._tinytex_init(dft, buf.byteLength, ptr);
        _mathFonts.push(fontName);
        _currentMathFont = fontName;
        _isLittleEndian = _runtime._tinytex_isLittleEndian();
      } finally {
        freeHeap(ptr);
        freeHeap(dft);
      }
    });
}

/** Release the context. */
context.release = function () {
  _runtime._tinytex_release();
}

/** Check if context is initialized. */
context.isInited = function () {
  return _runtime._tinytex_isInited();
}

/**
 * Add a main font to context.
 *
 * @param {String} familyName the font family name
 * @param {String} clmDataUri the clm data uri
 * @param {String} styleName the font style name,
 * e.g: rm(stands for roman), bf(stands for bold) etc
 */
context.addMainFont = function (familyName, clmDataUri, styleName) {
  return fetch(clmDataUri)
    .then(res => res.arrayBuffer())
    .then(buf => {
      const fname = _runtime.allocateUTF8(familyName);
      const sname = _runtime.allocateUTF8(styleName);
      const ptr = copyToHeap(buf);
      try {
        _runtime._tinytex_addMainFont(fname, sname, buf.byteLength, ptr);
        _mainFonts.push(familyName);
      } finally {
        freeHeap(fname);
        freeHeap(sname);
        freeHeap(ptr);
      }
      if (_currentMainFont === "") {
        this.setMainFont(familyName);
      }
      console.log(
        "add main font, family: '" + familyName
        + "', style: '" + styleName
        + "' successfully, size: "
        + buf.byteLength
      );
    })
}

/**
 * Set main font to render formulas. The font MUST be added to the context.
 *
 * @param {String} familyName the family name, the engine will use math font
 * to render text if is empty.
 * @throws {TypeError} if the font was not added.
 */
context.setMainFont = function (familyName) {
  if (familyName !== "" && !_mainFonts.includes(familyName)) {
    throw new TypeError("the font family `" + familyName + "` has no font.");
  }
  const cstr = _runtime.allocateUTF8(familyName);
  try {
    _runtime._tinytex_setDefaultMainFont(cstr);
  } finally {
    freeHeap(cstr);
  }
  _currentMainFont = familyName;
  console.log("set main font: " + familyName);
}

/**
 * Add a math font to context.
 *
 * @param {String} clmDataUri the clm data uri
 * @param {String} fontName the font name
 * @return {Promise<void>} a promise to add the math font
 * @throws {Error} if the given font name was added already or the
 * clm data can not be parsed
 */
context.addMathFont = function (clmDataUri, fontName) {
  if (_mathFonts.includes(fontName)) {
    throw new Error("font `" + fontName + "` was added already.");
  }
  return fetch(clmDataUri)
    .then(res => res.arrayBuffer())
    .then(buf => {
      const fname = _runtime.allocateUTF8(fontName);
      const ptr = copyToHeap(buf);
      try {
        _runtime._tinytex_addMathFont(fname, buf.byteLength, ptr);
        _mathFonts.push(fontName);
      } finally {
        freeHeap(fname);
        freeHeap(ptr);
      }
      console.log("add math font '" + fontName + "' successfully, size: " + buf.byteLength);
    });
}

/**
 * Set math font to render formulas. The font MUST be added to the context.
 *
 * @param {String} fontName the font name
 * @throws {TypeError} if the font was not added
 */
context.setMathFont = function (fontName) {
  if (!_mathFonts.includes(fontName)) {
    throw new TypeError("the font `" + fontName + "` was not added.");
  }
  const cstr = _runtime.allocateUTF8(fontName);
  try {
    _runtime._tinytex_setDefaultMathFont(cstr);
  } finally {
    freeHeap(cstr);
  }
  _currentMathFont = fontName;
  console.log("set math font: " + fontName);
}

/**
 * Get the math fonts added to the context, return an empty array if the
 * context was not initialized.
 *
 * @return {String[]} the copy of the loaded math fonts
 */
context.getMathFonts = function () {
  return [..._mathFonts];
}

/**
 * Parse a LaTeX formatted string.
 *
 * @param {string} tex the LaTeX formatted string to be parsed
 * @param {number} width the width of the graphical context
 * @param {number} textSize the text size to paint the formula
 * @param {number} lineSpace the line space to layout multi-lines formulas
 * @param {number} color \#AARRGGBB formatted color
 * @returns {Render} a render to paint the parsed formula
 */
context.parse = function (tex, width, textSize, lineSpace, color) {
  const cstr = _runtime.allocateUTF8(tex);
  let ptr = 0;
  try {
    ptr = _runtime._tinytex_parseRender(cstr, width, textSize, lineSpace, color);
  } finally {
    freeHeap(cstr);
  }
  return new Render(ptr, _isLittleEndian);
}

export {context}
