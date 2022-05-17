'use strict'

import {runtime} from "./runtime";

const textLayout = {};
const canvas = document.createElement("canvas");
textLayout.ctx = canvas.getContext('2d');
textLayout.map = new Map();
textLayout.id = 0;

function toCSSFont(desc) {
  return Object
    .entries(desc)
    .reduce((p, [k, v]) => {
      return p + " " + v;
    }, "");
}

/**
 * Get the text layout by its id.
 * @param {number} id
 */
function getTextLayout(id) {
  return textLayout.map.get(id);
}

/**
 * Create a text layout from given text and font.
 *
 * @param {number} src the string pointer from native side
 * @param {number} font the font description from native side
 * @return {number} the layout id
 */
function createTextLayout(src, font) {
  const txt = runtime.UTF8ToString(src);
  const desc = {};
  desc['font-weight'] = runtime._microtex_isBold(font) ? 'bold' : 'normal';
  desc['font-style'] = runtime._microtex_isItalic(font) ? 'italic' : 'normal';
  desc['font-size'] = runtime._microtex_fontSize(font) + "px";
  if (runtime._microtex_isSansSerif(font)) {
    desc['font-family'] = 'sans-serif';
  } else if (runtime._microtex_isMonospace(font)) {
    desc['font-family'] = 'monospace';
  } else {
    desc['font-family'] = 'serif';
  }

  const id = textLayout.id;
  textLayout.map.set(id, {
    font: desc,
    text: txt
  });
  textLayout.id++;
  return id;
}

/**
 * Get text layout bounds.
 *
 * @param {number} id the layout id
 * @param {number} ptr the bounds buffer from native
 */
function getTextLayoutBounds(id, ptr) {
  // the value must be exists
  const value = textLayout.map.get(id);
  const ctx = textLayout.ctx;
  ctx.font = toCSSFont(value.font);
  // measure the text and get its metrics
  const m = ctx.measureText(value.text);
  const w = m.width;
  const h = m.actualBoundingBoxAscent + m.actualBoundingBoxDescent;
  const a = -m.actualBoundingBoxAscent;
  runtime._microtex_setTextLayoutBounds(ptr, w, h, a);
}

/**
 * Release current text layout.
 *
 * @param {number} id the layout id
 */
function releaseTextLayout(id) {
  textLayout.map.delete(id);
}

export {
  toCSSFont, getTextLayout,
  createTextLayout, getTextLayoutBounds, releaseTextLayout
}
