'use strict'

import {runtime} from "./runtime";
import {getTextLayout, toCSSFont} from "./textlayout";

/**
 * Convert a \#AARRGGBB color to CSS color.
 *
 * @param {number} color the \#AARRGGBB color
 */
function toCSSColor(color) {
  const css = ((color << 8) >>> 0 | color >>> 24) >>> 0;
  let str = css.toString(16);
  if (str.length < 8) {
    for (let i = 8 - str.length; i > 0; i--) {
      str = '0' + str;
    }
  }
  return `#${str}`;
}

/**
 * Array to get command arguments.
 *
 * @type {Float32Array}
 */
const argBuf = new Float32Array(8);

/** Extension method to support with font. */
CanvasRenderingContext2D.prototype.fillTextWithFont = function (txt, x, y, font) {
  this.fillText(txt, x, y);
}

/** Extension method to support with id. */
CanvasRenderingContext2D.prototype.beginPathWithId = function (id) {
  this.beginPath();
}

CanvasRenderingContext2D.prototype.fillPathWithId = function (id) {
  this.fill();
}

/**
 * Create a Render to paint formulas. You MUST call {@link Render.release} to free
 * memory after this render is unused.
 *
 * @param {number} nativeRender the pointer of native render parsed by the engine
 * @param {boolean} isLittleEndian if the byte order is little endian
 * @constructor
 */
function Render(nativeRender, isLittleEndian) {
  const _nativeRender = nativeRender;
  const _isLittleEndian = isLittleEndian;

  /**
   * Get the paintable formula width.
   *
   * @return {number}
   */
  this.getWidth = function () {
    return runtime._microtex_getRenderWidth(_nativeRender);
  }

  /**
   * Get the height of the render.
   *
   * @return {number}
   */
  this.getHeight = function () {
    return runtime._microtex_getRenderHeight(_nativeRender);
  }

  /**
   * Get distance above baseline.
   *
   * @return {number}
   */
  this.getBaseline = function () {
    return this.getHeight() - this.getDepth();
  }

  /**
   * Get the depth below the baseline, in positive.
   *
   * @return {number}
   */
  this.getDepth = function () {
    return runtime._microtex_getRenderDepth(_nativeRender);
  }

  const capMap = {0: "butt", 1: "round", 2: "square"};
  const joinMap = {0: "bevel", 1: "round", 2: "miter"};

  /**
   * Draw the formula on point (x, y).
   *
   * @param ctx the graphical (2D) context
   * @param {number} x the x coordinate
   * @param {number} y the y coordinate
   */
  this.draw = function (ctx, x, y) {
    const ptr = runtime._microtex_getDrawingData(_nativeRender, x, y);
    // make a view to iterate over drawing commands
    const v = new DataView(runtime.HEAPU8.buffer);
    let offset = 0;

    function getU8() {
      const x = v.getUint8(ptr + offset);
      offset += 1;
      return x;
    }

    function getNum(size, f) {
      const x = f(ptr + offset, _isLittleEndian);
      offset += size;
      return x;
    }

    const getU16 = () => getNum(2, v.getUint16.bind(v));
    const getI32 = () => getNum(4, v.getInt32.bind(v));
    const getU32 = () => getNum(4, v.getUint32.bind(v));
    const getF32 = () => getNum(4, v.getFloat32.bind(v));

    function getF32s(cnt) {
      for (let j = 0; j < cnt; j++) {
        argBuf[j] = v.getFloat32(ptr + offset, _isLittleEndian);
        offset += 4;
      }
      return argBuf;
    }

    function forwardString() {
      while (getU8() !== 0);
    }

    /** Make a round rectangle. */
    function roundRect(x, y, w, h, rx, ry) {
      const r = Math.min(rx, ry, w / 2, h / 2);
      ctx.beginPath();
      ctx.moveTo(x + r, y);
      ctx.lineTo(x + w - r, y);
      ctx.quadraticCurveTo(x + w, y, x + w, y + r);
      ctx.lineTo(x + w, y + h - r);
      ctx.quadraticCurveTo(x + w, y + h, x + w - r, y + h);
      ctx.lineTo(x + r, y + h);
      ctx.quadraticCurveTo(x, y + h, x, y + h - r);
      ctx.lineTo(x, y + r);
      ctx.quadraticCurveTo(x, y, x + r, y);
      ctx.closePath();
    }

    const len = getU32();

    let sx = 1;
    let sy = 1;
    while (offset < len) {
      const cmd = getU8();
      switch (cmd) {
        case 0: // setColor
          const color = getU32();
          const css = toCSSColor(color);
          ctx.fillStyle = css;
          ctx.strokeStyle = css;
          break;
        case 1: // setStroke
          const lineWidth = getF32();
          const miterLimit = getF32();
          const cap = getU32();
          const join = getU32();
          ctx.lineWidth = lineWidth;
          ctx.miterLimit = miterLimit;
          ctx.lineCap = capMap[cap];
          ctx.lineJoin = joinMap[join];
          break;
        case 2: // setDash
          const hasDash = getU8() === 1;
          const dash = hasDash ? [5 / sx, 5 / sx] : [];
          ctx.setLineDash(dash);
          break;
        case 3: // setFont
          // TODO does not support font yet
          // since CanvasRenderingContext2D does not support glyph drawing
          // consume the bytes
          forwardString();
          break;
        case 4: // setFontSize
          // TODO does not support font yet
          // since CanvasRenderingContext2D does not support glyph drawing
          // consume the bytes
          getF32();
          break;
        case 5: // translate
          const t = getF32s(2);
          ctx.translate(t[0], t[1]);
          break;
        case 6: // scale
          const s = getF32s(2);
          sx *= s[0];
          sy *= s[1];
          ctx.scale(s[0], s[1]);
          break;
        case 7: // rotate
          const r = getF32s(3);
          ctx.translate(r[1], r[2]);
          ctx.rotate(r[0]);
          ctx.translate(-r[1], -r[2]);
          break;
        case 8: // reset
          ctx.setTransform(1, 0, 0, 1, 0, 0);
          break;
        case 9: // drawGlyph
          // TODO: does not support draw glyph yet
          // since CanvasRenderingContext2D does not support glyph drawing
          // consume the bytes
          getU16();
          getF32s(2);
          break;
        case 10: // beginPath
          const pid = getI32();
          ctx.beginPathWithId(pid);
          break;
        case 11: // moveTo
          const m = getF32s(2);
          ctx.moveTo(m[0], m[1]);
          break;
        case 12: // lineTo
          const l = getF32s(2);
          ctx.lineTo(l[0], l[1]);
          break;
        case 13: // cubicTo
          const c = getF32s(6);
          ctx.bezierCurveTo(c[0], c[1], c[2], c[3], c[4], c[5]);
          break;
        case 14: // quadTo
          const q = getF32s(4);
          ctx.quadraticCurveTo(q[0], q[1], q[2], q[3]);
          break;
        case 15: // closePath
          ctx.closePath();
          break;
        case 16: // fillPath
          const pathId = getI32();
          ctx.fillPathWithId(pathId);
          break;
        case 17: // drawLine
          const dl = getF32s(4);
          ctx.beginPath();
          ctx.moveTo(dl[0], dl[1]);
          ctx.lineTo(dl[2], dl[3]);
          ctx.stroke();
          break;
        case 18: // drawRect
          const dr = getF32s(4);
          ctx.strokeRect(dr[0], dr[1], dr[2], dr[3]);
          break;
        case 19: // fillRect
          const fr = getF32s(4);
          ctx.fillRect(fr[0], fr[1], fr[2], fr[3]);
          break;
        case 20: // drawRoundRect
          const rr = getF32s(6);
          roundRect(rr[0], rr[1], rr[2], rr[3], rr[4], rr[5]);
          ctx.stroke();
          break;
        case 21: // fillRoundRect
          const rf = getF32s(6);
          roundRect(rf[0], rf[1], rf[2], rf[3], rf[4], rf[5]);
          ctx.fill();
          break;
        case 22: // drawTextLayout
          const id = getU32();
          const xy = getF32s(2);
          const old = ctx.font;
          const value = getTextLayout(id);
          ctx.font = toCSSFont(value.font);
          ctx.fillTextWithFont(value.text, xy[0], xy[1], value.font);
          ctx.font = old;
          break;
        default:
          // invalid drawing command
          break;
      }
    }
    runtime._free(ptr);
  }

  /** Release the native render. */
  this.release = function () {
    runtime._microtex_deleteRender(nativeRender);
  }
}

export {Render}
