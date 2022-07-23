'use strict'

import ace from 'ace-builds';
import 'ace-builds/src-min-noconflict/mode-latex';
import * as microtex from "../dist/microtex"
import {examples} from "./examples";
import * as utils from "./utils"
import {helloDemo} from "./hello";

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

microtex
  .context
  .init(fonts[0])
  .then(_ => {
    console.log("init finished...");
    const str = hello();
    initEditor(str);
    listenResize();
    listenEvents();
  })
  .then(_ => {
    const p = fonts.slice(1).map(path => microtex.context.addFont(path));
    return Promise.all(p);
  })
  .then(_ => {
    initFontOptions();
  });

function hello() {
  let hello = helloDemo;
  const arg = utils.getQueryParam("tex");
  if (arg != null) {
    hello = arg;
    if (arg.startsWith('"')) hello = hello.substring(1);
    if (arg.endsWith('"')) hello = hello.substring(0, hello.length - 1);
    const mode = document.getElementById("mode");
    mode.selectedIndex = 1;
  }
  parse(hello);
  return hello;
}

function initFontOptions() {
  let selectMain = document.getElementById('main-font');
  microtex.context.getMainFontFamilyNames().forEach(f => {
    selectMain.add(new Option(f, f));
  })
  // special case: none fallback to math font
  selectMain.add(new Option("none", "none"));

  let selectMath = document.getElementById('fonts')
  microtex.context.getMathFontNames().forEach(f => {
    selectMath.add(new Option(f, f));
  });
}

function initEditor(str) {
  editor = ace.edit("editor");
  editor.session.setMode("ace/mode/latex");
  editor.getSession().setTabSize(2);
  editor.setValue(str);
  editor.getSession().on('change', () => parse(editor.getValue()));
}

function listenEvents() {
  document.getElementById('main-font').onchange = () => {
    const option = document.getElementById("main-font").value;
    const name = option === "none" ? "" : option;
    microtex.context.setMainFont(name);
    parse(editor.getValue());
  };
  document.getElementById('fonts').onchange = () => {
    const option = document.getElementById("fonts").value;
    microtex.context.setMathFont(option);
    parse(editor.getValue());
  };
  document.getElementById('textsize').onchange = () => {
    parse(editor.getValue());
  };
  document.getElementById('next').onclick = () => {
    if (!examples.isInited()) return;
    let str = examples.next();
    parse(str);
    editor.setValue(str);
  };
  document.getElementById('mode').onchange = () => {
    parse(editor.getValue());
  };
  document.getElementById('rendering-mode').onchange = () => {
    parse(editor.getValue());
  };
}

function listenResize() {
  const left = document.getElementById('left');
  const right = document.getElementById('right');
  const section = document.getElementById('section');
  const resize = document.getElementById('pan-resize');
  let isDown = 0;
  resize.addEventListener('mousedown', () => {
    isDown = 1;
    const move = e => {
      if (isDown === 1) {
        const w = section.getBoundingClientRect().width;
        let x = e.clientX - section.getBoundingClientRect().left;
        x = Math.min(Math.max(x, 100), w - 100);
        left.style.flexBasis = x + "px";
        right.style.flexBasis = (w - x) + "px";
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

function parse(str) {
  const wrapper = document.getElementById("wrapper");
  wrapper.removeAllChildren();
  const mode = document.getElementById("mode").value;
  if (mode === "text") {
    parseText(wrapper, str);
  } else {
    parseRender(str, wrapper);
  }
}

function parseText(container, str) {
  const size = +document.getElementById("textsize").value;

  let k = 0;
  let p = createParagraph();

  function createParagraph() {
    let p = document.createElement("p");
    p.style.fontSize = size + "px";
    return p;
  }

  function appendText(end) {
    const t = document.createTextNode(str.substring(k, end));
    p.append(t);
    k = end;
  }

  for (let i = 0; i < str.length; i++) {
    const c = str[i];
    if (c === '\n' && i + 1 < str.length && str[i + 1] === '\n') {
      i += 2;
      appendText(i);
      container.append(p);
      p = createParagraph();
    } else if (c === '$') {
      let tex = "";
      let inline = true;
      if (i + 1 < str.length && str[i + 1] === '$') {
        inline = false;
        tex = str.takeUntil((j, x) => j > i + 1 && x === '$' && str[j - 1] === '$', i, true);
      } else {
        inline = true;
        tex = str.takeUntil((j, x) => j > i && x === '$', i, true);
      }
      if (tex.length <= 0) continue;
      appendText(i);
      parseRender(tex, p, 2, inline);
      i += tex.length - 1;
      k += tex.length;
    }
  }

  appendText(str.length);
  container.append(p);
}

function parseRender(str, parent, padding = 8, inline = true) {
  // get text size
  const size = +document.getElementById("textsize").value * 1.2;
  // get content width
  let width = document.getElementById("wrapper").offsetWidth - padding * 2;

  let r = null;
  try {
    r = microtex.context.parse(str, width, size, size / 3, 0xff3b3b3b, false);
  } catch (e) {
    console.log(e);
    return;
  }

  const drawCanvas = () => {
    const canvas = parent.attachCanvas(r, padding, inline);
    adjustStyle(canvas, inline);
    const ctx = canvas.getContext('2d');
    ctx.scale(window.devicePixelRatio, window.devicePixelRatio);
    r.draw(ctx, padding, padding);
  };

  const drawSvg = () => {
    const svg = new microtex.SvgCanvas(r.getWidth() + padding * 2, r.getHeight() + padding * 2);
    r.draw(svg, padding, padding);
    const e = svg.getSvg();
    e.style.verticalAlign = (-r.getDepth() - padding) + "px";
    adjustStyle(e, inline);
    parent.appendChild(e);
  };

  // rendering mode
  const mode = document.getElementById('rendering-mode').value;
  console.log(`rendering mode: ${mode}`)
  if (mode === 'Canvas') {
    drawCanvas();
  } else {
    drawSvg();
  }

  r.release();
}

function adjustStyle(e, inline) {
  if (inline) return;
  e.style.display = "block";
  e.style.marginLeft = "auto";
  e.style.marginRight = "auto";
  e.style.marginTop = "8px";
  e.style.marginBottom = "8px";
}
