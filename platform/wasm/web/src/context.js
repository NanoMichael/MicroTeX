'use strict'

import {initRuntime} from "./runtime";
import {Render} from './render';

let context = {};

/**
 * The loaded font names.
 *
 * @type {String[]}
 * @private
 */
let _mathFonts = [];

/**
 * The current font name.
 *
 * @type {string}
 * @private
 */
let _currentFont = "";

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
  if (_runtime != null && _runtime._clatex_isInited()) {
    throw new Error("the context was initialized already.");
  }
  return initRuntime()
    .then(r => _runtime = r)
    .then(_ => fetch(clmDataUri))
    .then(res => res.arrayBuffer())
    .then(buf => {
      console.log(fontName + " clm data size: " + buf.byteLength);
      const dft = _runtime.allocateUTF8(fontName);
      const ptr = copyToHeap(buf);
      try {
        _runtime._clatex_init(dft, buf.byteLength, ptr);
        _mathFonts.push(fontName);
        _currentFont = fontName;
        _isLittleEndian = _runtime._clatex_isLittleEndian();
      } finally {
        freeHeap(ptr);
        freeHeap(dft);
      }
    });
}

/** Release the context. */
context.release = function () {
  _runtime._clatex_release();
}

/** Check if context is initialized. */
context.isInited = function () {
  return _runtime._clatex_isInited();
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
      console.log("add math font, clm data size: " + buf.byteLength);
      const fname = _runtime.allocateUTF8(fontName);
      const ptr = copyToHeap(buf);
      try {
        _runtime._clatex_addMathFont(fname, buf.byteLength, ptr);
        _mathFonts.push(fontName);
      } finally {
        freeHeap(fname);
        freeHeap(ptr);
      }
      console.log("add math font " + fontName + "successfully...");
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
  _currentFont = fontName;
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
context.parseRender = function (tex, width, textSize, lineSpace, color) {
  const cstr = _runtime.allocateUTF8(tex);
  let ptr = 0;
  try {
    ptr = _runtime._clatex_parseRender(cstr, width, textSize, lineSpace, color);
  } finally {
    freeHeap(cstr);
  }
  return new Render(ptr, _isLittleEndian);
}

export {context}
