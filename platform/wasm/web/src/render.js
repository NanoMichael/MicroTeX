'use strict'

import {runtime} from "./runtime";

/**
 * Convert a \#AARRGGBB color to CSS color.
 *
 * @param {Number} color the \#AARRGGBB color
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

/**
 * Create a Render to paint formulas. You MUST call {@link Render.release} to free
 * memory after this render is unused.
 *
 * @param {Number} nativeRender the pointer of native render parsed by the engine
 * @param {boolean} isLittleEndian if the byte order is little endian
 * @constructor
 */
function Render(nativeRender, isLittleEndian) {
  const _nativeRender = nativeRender;
  const _isLittleEndian = isLittleEndian;

  /**
   * Get the paintable formula width.
   *
   * @return {Number}
   */
  this.getWidth = function () {
    return runtime._microtex_getRenderWidth(_nativeRender);
  }

  /**
   * Get the height of the render.
   *
   * @return {Number}
   */
  this.getHeight = function () {
    return runtime._microtex_getRenderHeight(_nativeRender);
  }

  /**
   * Get the depth below the baseline, in positive.
   *
   * @return {Number}
   */
  this.getDepth = function () {
    return runtime._microtex_getRenderDepth(_nativeRender);
  }

  const capMap = {0: "butt", 1: "round", 2: "square"};
  const joinMap = {0: "bevel", 1: "round", 2: "miter"};

  /**
   * Draw the formula on point (x, y).
   *
   * @param {CanvasRenderingContext2D} ctx the graphical (2D) context
   * @param {Number} x the x coordinate
   * @param {Number} y the y coordinate
   */
  this.draw = function (ctx, x, y) {
    const ptr = runtime._microtex_getDrawingData(_nativeRender, x, y);
    // make a view to iterate drawing commands
    const v = new DataView(runtime.HEAPU8.buffer);
    let offset = 0;

    function getU8() {
      const x = v.getUint8(ptr + offset);
      offset += 1;
      return x;
    }

    function getU32() {
      const x = v.getUint32(ptr + offset, _isLittleEndian);
      offset += 4;
      return x;
    }

    function getF32() {
      const x = v.getFloat32(ptr + offset, _isLittleEndian);
      offset += 4;
      return x;
    }

    function getF32s(cnt) {
      for (let j = 0; j < cnt; j++) {
        argBuf[j] = v.getFloat32(ptr + offset, _isLittleEndian);
        offset += 4;
      }
      return argBuf;
    }

    /** Make a round rectangle. */
    function roundRect(x, y, w, h, rx, ry) {
      const r = Math.max(rx, ry);
      const d = Math.PI / 180;
      ctx.beginPath();
      ctx.arc(x + r, y + r, r, 180 * d, 270 * d);
      ctx.arc(x + w - r, y + r, r, -90 * d, 0);
      ctx.arc(x + w - r, y + h - r, r, 0, 90 * d);
      ctx.arc(x + r, y + h - r, r, 90 * d, 180 * d);
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
        case 2: // translate
          const t = getF32s(2);
          ctx.translate(t[0], t[1]);
          break;
        case 3: // scale
          const s = getF32s(2);
          sx *= s[0];
          sy *= s[1];
          ctx.scale(s[0], s[1]);
          break;
        case 4: // rotate
          const r = getF32s(3);
          ctx.translate(r[1], r[2]);
          ctx.rotate(r[0]);
          ctx.translate(-r[1], -r[2]);
          break;
        case 5: // reset
          ctx.setTransform(1, 0, 0, 1, 0, 0);
          break;
        case 6: // moveTo
          const m = getF32s(2);
          ctx.moveTo(m[0], m[1]);
          break;
        case 7: // lineTo
          const l = getF32s(2);
          ctx.lineTo(l[0], l[1]);
          break;
        case 8: // cubicTo
          const c = getF32s(6);
          ctx.bezierCurveTo(c[0], c[1], c[2], c[3], c[4], c[5]);
          break;
        case 9: // quadTo
          const q = getF32s(4);
          ctx.quadraticCurveTo(q[0], q[1], q[2], q[3]);
          break;
        case 10: // closePath
          ctx.closePath();
          break;
        case 11: // fillPath
          ctx.fill();
          ctx.beginPath();
          break;
        case 12: // drawLine
          const dl = getF32s(4);
          ctx.beginPath();
          ctx.moveTo(dl[0], dl[1]);
          ctx.lineTo(dl[2], dl[3]);
          ctx.stroke();
          break;
        case 13: // drawRect
          const dr = getF32s(4);
          ctx.strokeRect(dr[0], dr[1], dr[2], dr[3]);
          break;
        case 14: // fillRect
          const fr = getF32s(4);
          ctx.fillRect(fr[0], fr[1], fr[2], fr[3]);
          break;
        case 15: // drawRoundRect
          const rr = getF32s(6);
          roundRect(rr[0], rr[1], rr[2], rr[3], rr[4], rr[5]);
          ctx.stroke();
          break;
        case 16: // fillRoundRect
          const rf = getF32s(6);
          roundRect(rf[0], rf[1], rf[2], rf[3], rf[4], rf[5]);
          ctx.fill();
          break;
        case 17: // beginPath
          ctx.beginPath();
          break;
        case 18: // drawTextLayout
          const id = getU32();
          const xy = getF32s(2);
          const old = ctx.font;
          const value = runtime.textLayout.map.get(id);
          ctx.font = value.font;
          ctx.fillText(value.text, xy[0], xy[1]);
          ctx.font = old;
          break;
        case 19: // setDash
          const hasDash = getU8() === 1;
          const dash = hasDash ? [5 / sx, 5 / sx] : [];
          ctx.setLineDash(dash);
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
