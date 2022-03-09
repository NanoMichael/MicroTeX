'use strict'

import ace from 'ace-builds';
import 'ace-builds/src-min-noconflict/mode-latex';
import {context} from '../dist/tinytex';

let editor = null

const fonts = [
  "./res/xits/XITSMath-Regular.clm2",
  "./res/lm-math/latinmodern-math.clm2",
  "./res/tex-gyre/texgyredejavu-math.clm2",
  "./res/firamath/FiraMath-Regular.clm2",
  "./res/xits/XITS-Regular.clm2",
  "./res/xits/XITS-Italic.clm2",
  "./res/xits/XITS-Bold.clm2",
  "./res/xits/XITS-BoldItalic.clm2"
];

context
  .init(fonts[0])
  .then(_ => {
    const p = fonts.slice(1).map(path => context.addFont(path))
    return Promise.all(p);
  })
  .then(_ => {
    console.log("init & add font finished")
    const str = hello();
    initFontOptions();
    initEditor(str);
    listenEvents()
  });

function hello() {
  let hello = `\\text{Hello from Tiny\\kern-.1em\\TeX 🥰, have fun!}`;
  const arg = getQueryParam("tex");
  if (arg != null) {
    hello = arg;
    if (arg.startsWith('"')) hello = hello.substring(1);
    if (arg.endsWith('"')) hello = hello.substring(0, hello.length - 1);
  }
  parse(hello);
  return hello;
}

function initFontOptions() {
  let selectMain = document.getElementById('main-font');
  context.getMainFontFamilyNames().forEach(f => {
    selectMain.add(new Option(f, f));
  })
  // special case: none fallback to math font
  selectMain.add(new Option("none", "none"));

  let selectMath = document.getElementById('fonts')
  context.getMathFontNames().forEach(f => {
    selectMath.add(new Option(f, f));
  })
}

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
  // resize
  const left = document.getElementById('left');
  const section = document.getElementById('section');
  const resize = document.getElementById('pan-resize');
  let isDown = 0;
  resize.addEventListener('mousedown', () => {
    isDown = 1;
    const move = e => {
      if (isDown === 1) {
        const x = e.clientX - section.getBoundingClientRect().left;
        left.style.flexBasis = x + "px";
      } else {
        end();
      }
    };
    const end = () => {
      isDown = 0;
      document.body.removeEventListener('mousemove', move);
      document.body.removeEventListener('mouseup', end);
    };
    document.body.addEventListener('mousemove', move);
    document.body.addEventListener('mouseup', end);
  });
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

function getQueryParam(name) {
  const reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
  const r = window.location.search.substr(1).match(reg);
  if (r != null) return unescape(r[2]);
  return null;

}
