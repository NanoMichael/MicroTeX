'use strict'

HTMLElement.prototype.removeAllChildren = function () {
  while (this.firstChild) {
    this.removeChild(this.firstChild);
  }
}

HTMLCanvasElement.prototype.fixDpi = function () {
  const dpi = window.devicePixelRatio || 1;
  if (dpi === 1) return;
  const w = +this.style.width.slice(0, -2);
  const h = +this.style.height.slice(0, -2);
  this.width = Math.floor(w * dpi);
  this.height = Math.floor(h * dpi);
}

HTMLElement.prototype.attachCanvas = function (render, padding, inline = true) {
  const p = padding * 2;
  const canvas = document.createElement("canvas");
  canvas.style.width = (render.getWidth() + p) + "px";
  canvas.style.height = (render.getHeight() + p) + "px";
  canvas.width = render.getWidth() + p;
  canvas.height = render.getHeight() + p;
  canvas.style.verticalAlign = (-render.getDepth() - padding) + "px";
  this.appendChild(canvas);
  canvas.fixDpi();
  return canvas;
}

String.prototype.takeUntil = function (stop, from = 0, inclusive = false) {
  for (let i = from; i < this.length; i++) {
    if (stop(i, this[i])) {
      return this.substring(from, inclusive ? i + 1 : i);
    }
  }
  return "";
}

function getQueryParam(name) {
  const reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
  const r = window.location.search.substr(1).match(reg);
  if (r != null) return unescape(r[2]);
  return null;
}

export {getQueryParam};
