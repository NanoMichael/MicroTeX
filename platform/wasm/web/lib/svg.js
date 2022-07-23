'use strict'

/**
 * @param {number} width
 * @param {number} height
 * @return {SVGSVGElement}
 */
function createSvg(width, height) {
  let svg = document.createElementNS("http://www.w3.org/2000/svg", "svg");
  svg.setAttribute("version", "1.1");
  svg.setAttribute("xmlns", "http://www.w3.org/2000/svg");
  svg.setAttribute("width", `${width}`);
  svg.setAttribute("height", `${height}`);
  svg.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:xlink", "http://www.w3.org/1999/xlink");
  return svg;
}

/**
 * The loaded path ids
 *
 * @type {Set<string>}
 * @private
 */
const _pathIds = new Set();

function getOrCreatePhantomDefs() {
  const id = "MicroTeX-path-cache";
  let svg = document.getElementById(id);
  if (!svg) {
    svg = createSvg(0, 0);
    svg.setAttribute("id", id);
    const defs = createElement("defs", {"id": "MicroTeX-path-cache-defs"});
    svg.appendChild(defs);
    document.body.appendChild(svg);
    return defs;
  }
  return svg.getElementById("MicroTeX-path-cache-defs");
}

function saveAndCreatePathElement(id, data) {
  // Does not have an id, create a `path` element
  if (id < 0) {
    return !data ? null : createElement("path", {"d": data});
  }
  const pid = `MicroTeX-path-${id}`;
  if (!_pathIds.has(id)) {
    // First, create a `path` element and save into defs
    _pathIds.add(id);
    const path = createElement("path", {"d": data, "id": pid});
    const defs = getOrCreatePhantomDefs();
    defs.appendChild(path);
  }
  // Second, return an `use` element
  return createElement("use", {"href": `#${pid}`});
}

/**
 * Create a svg element.
 *
 * @param {string} name the name of the element
 * @param {Object} properties
 */
function createElement(name, properties = {}) {
  const element = document.createElementNS("http://www.w3.org/2000/svg", name);
  Object.entries(properties).forEach(([k, v]) => element.setAttribute(k, v.toString()));
  return element;
}

/**
 * Create a SVG canvas to draw elements.
 *
 * @param {number} width
 * @param {number} height
 * @constructor
 */
function SvgCanvas(width, height) {
  let _root = createSvg(width, height);

  let _sx = 1, _sy = 1;
  let _dx = 0, _dy = 0;

  let _isCurrentPathCached = false;
  let _currentPathId = -1;
  let _currentPath = "";

  function addPathCmd(cmd, ...args) {
    _currentPath += cmd;
    for (let i = 0; i < args.length; i += 2) {
      _currentPath += args[i] + " ";
      _currentPath += args[i + 1] + " ";
    }
  }

  let _rotation = {r: 0, x: 0, y: 0};

  this.fillStyle = "black";
  this.strokeStyle = "black";
  this.lineWidth = 1;
  this.miterLimit = 0;
  this.lineCap = "butt";
  this.lineJoin = "bevel";
  this.font = "20px Arial";
  this.dash = [];

  this.applyNode = function (node, fill = false) {
    if (node === null) return;
    const e = node;
    _root.appendChild(e);
    let t = "";
    if (_dx !== 0 || _dy !== 0) {
      t += `translate(${_dx},${_dy})`;
    }
    if (_rotation.r !== 0) {
      t += `rotate(${_rotation.r * 180 / Math.PI},${_rotation.x - _dx},${_rotation.y - _dy})`;
    }
    if (_sx !== 1 || _sy !== 1) {
      t += `scale(${_sx},${_sy})`;
    }
    if (t !== "") {
      e.setAttribute("transform", t);
    }
    if (fill) {
      e.setAttribute("fill", this.fillStyle);
    } else {
      e.setAttribute("stroke", this.strokeStyle);
      e.setAttribute("stroke-width", `${this.lineWidth}`);
      e.setAttribute("fill", "none");
      if (this.dash.length > 0) {
        e.setAttribute("stroke-dasharray", this.dash.toString());
      }
    }
  }

  this.translate = function (dx, dy) {
    _dx += dx * _sx;
    _dy += dy * _sy;
  }

  this.scale = function (sx, sy) {
    _sx *= sx;
    _sy *= sy;
  }

  this.rotate = function (angle) {
    _rotation.r += angle;
    _rotation.x = _dx;
    _rotation.y = _dy;
  }

  this.setTransform = function () {
    _sx = _sy = 1;
    _dx = _dy = 0;
  }

  this.beginPathWithId = function (id) {
    _isCurrentPathCached = _pathIds.has(id);
    _currentPathId = id;
    _currentPath = "";
  }

  this.beginPath = function () {
    this.beginPathWithId(-1);
  }

  this.moveTo = function (x, y) {
    if (!_isCurrentPathCached) addPathCmd('M', x, y);
  }

  this.lineTo = function (x, y) {
    if (!_isCurrentPathCached) addPathCmd('L', x, y);
  }

  this.bezierCurveTo = function (x1, y1, x2, y2, x, y) {
    if (!_isCurrentPathCached) addPathCmd('C', x1, y1, x2, y2, x, y);
  }

  this.quadraticCurveTo = function (x1, y1, x, y) {
    if (!_isCurrentPathCached) addPathCmd('Q', x1, y1, x, y);
  }

  this.closePath = function () {
    if (!_isCurrentPathCached) addPathCmd('Z');
  }

  this.flush = function (fill) {
    const e = saveAndCreatePathElement(_currentPathId, _currentPath);
    this.applyNode(e, fill);
    _currentPathId = -1;
    _currentPath = "";
  }

  this.fill = () => this.flush(true);

  this.fillPathWithId = (id) => this.flush(true);

  this.stroke = () => this.flush(false);

  this.rect = function (x, y, w, h) {
    this.beginPath();
    this.moveTo(x, y);
    this.lineTo(x + w, y);
    this.lineTo(x + w, y + h);
    this.lineTo(x, y + h);
    this.closePath();
  }

  this.strokeRect = function (x, y, w, h) {
    this.rect(x, y, w, h);
    this.stroke();
  }

  this.fillRect = function (x, y, w, h) {
    this.rect(x, y, w, h);
    this.fill();
  }

  this.fillTextWithFont = function (txt, x, y, font) {
    let e = createElement("text");
    e.textContent = txt;
    Object
      .entries(font)
      .forEach(([k, v]) => e.setAttribute(k, v.toString()));
    this.translate(x, y);
    this.applyNode(e, true);
    this.translate(-x, -y);
  }

  this.setLineDash = function (segments) {
    this.dash = segments;
  }

  this.getSvg = function () {
    return _root;
  }
}

export default SvgCanvas;
