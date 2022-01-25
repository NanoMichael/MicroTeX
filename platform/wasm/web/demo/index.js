'use strict'

import ace from 'ace-builds';
import 'ace-builds/src-min-noconflict/mode-latex';
import {context} from '../dist/tinytex';

let editor = null

// the math fonts
const mathFonts = {
  "xits": "./res/xits/XITSMath-Regular-path.clm",
  "lm": "./res/lm-math/latinmodern-math-path.clm",
  "dejavu": "./res/tex-gyre/texgyredejavu-math-path.clm",
  "fm": "./res/firamath/FiraMath-Regular-path.clm"
};
// the main fonts
const mainFonts = {
  "xits": {
    "rm": "./res/xits/XITS-Regular-path.clm",
    "it": "./res/xits/XITS-Italic-path.clm",
    "bf": "./res/xits/XITS-Bold-path.clm",
    "bfit": "./res/xits/XITS-BoldItalic-path.clm"
  }
}

context
  .init(mathFonts["xits"], "xits")
  .then(_ => {
    Object.entries(mathFonts).forEach(([name, path]) => {
      // the 'xits' was already loaded while init
      // ignore the result, we do it in parallel
      if (name !== "xits") context.addMathFont(path, name);
    });
  })
  .then(_ => {
    const xits = mainFonts.xits;
    Object.entries(xits).forEach(([style, path]) => {
      // ignore the result, we do it in parallel
      context.addMainFont("xits", path, style);
    });
  })
  .then(_ => {
    const hello = `\\text{Hello from Tiny\\kern-.1em\\TeX 🥰, have fun!}`;
    parse(hello);
    initEditor(hello);
    listenEvents()
  });

function initEditor(str) {
  editor = ace.edit("editor");
  editor.session.setMode("ace/mode/latex");
  editor.getSession().setTabSize(2);
  editor.setValue(str);
  editor.getSession().on('change', () => {
    if (context.isInited()) {
      parse(editor.getValue());
    }
  });
}

function listenEvents() {
  document.getElementById('main-font').onchange = e => {
    if (!context.isInited()) return;
    const option = document.getElementById("main-font").value;
    const name = option === "none" ? "" : option;
    context.setMainFont(name);
    parse(editor.getValue());
  };
  document.getElementById('fonts').onchange = e => {
    if (!context.isInited()) return;
    const option = document.getElementById("fonts").value;
    context.setMathFont(option);
    parse(editor.getValue());
  };
  document.getElementById('textsize').onchange = e => {
    if (!context.isInited()) return;
    parse(editor.getValue());
  };
  document.getElementById('next').onclick = e => {
    if (!examples.isInited() || !context.isInited()) return;
    let str = examples.next();
    parse(str);
    editor.setValue(str);
  };
}

function fixDpi(canvas) {
  let dpi = window.devicePixelRatio || 1;
  if (dpi === 1) return;
  let sw = +getComputedStyle(canvas).getPropertyValue("width").slice(0, -2);
  let sh = +getComputedStyle(canvas).getPropertyValue("height").slice(0, -2);
  console.log(`style size: ${sw}, ${sh}, ${dpi}`);
  // scale
  canvas.width = sw * dpi;
  canvas.height = sh * dpi;
}

const padding = 16;

function parse(str) {
  // get text size
  const sizeStr = document.getElementById("textsize").value;
  const size = parseInt(sizeStr);
  // get content width
  let width = document.getElementById("left").offsetWidth - padding;

  let r = null;
  try {
    r = context.parse(str, width, size, size / 3, 0x3b3b3b);
  } catch (e) {
    console.log(e);
    return;
  }

  const wrapper = document.getElementById("wrapper");
  if (wrapper.lastChild) {
    wrapper.removeChild(wrapper.lastChild);
  }

  function attach() {
    const canvas = document.createElement("canvas");
    canvas.style.width = (r.getWidth() + padding) + "px";
    canvas.style.height = (r.getHeight() + padding) + "px";
    canvas.width = r.getWidth() + padding;
    canvas.height = r.getHeight() + padding;
    // append to left
    wrapper.appendChild(canvas);
    // fix dpi
    fixDpi(canvas);
    return canvas;
  }

  // draw the render
  function draw(canvas) {
    const ctx = canvas.getContext('2d');
    r.draw(ctx, padding / 2, padding / 2);
  }

  const canvas = attach();
  draw(canvas);

  r.release();
}

function Examples(examplesUrl) {
  let isInited = false;

  this.isInited = function () {
    return isInited;
  }

  fetch(examplesUrl)
    .then(res => res.text())
    .then(txt => readExamples(txt));

  let samples = [];
  let i = -1;

  function readExamples(txt) {
    const lines = txt.split(/\r?\n/);
    let str = "";
    lines.forEach(v => {
      if (/^%+$/.test(v)) {
        samples.push(str);
        str = "";
      } else {
        str += v + "\n";
      }
    });
    isInited = true;
  }

  this.count = function () {
    return samples.length;
  }

  this.next = function () {
    i = (i + 1) % this.count();
    return samples[i];
  }
}

const examples = new Examples('./res/SAMPLES.tex');
