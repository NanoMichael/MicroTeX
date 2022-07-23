'use strict'

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

export {examples};
